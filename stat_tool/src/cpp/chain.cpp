/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       AMAPmod: Exploring and Modeling Plant Architecture
 *
 *       Copyright 1995-2002 UMR Cirad/Inra Modelisation des Plantes
 *
 *       File author(s): Y. Guedon (yann.guedon@cirad.fr)
 *
 *       $Source$
 *       $Id$
 *
 *       Forum for AMAPmod developers: amldevlp@cirad.fr
 *
 *  ----------------------------------------------------------------------------
 *
 *                      GNU General Public Licence
 *
 *       This program is free software; you can redistribute it and/or
 *       modify it under the terms of the GNU General Public License as
 *       published by the Free Software Foundation; either version 2 of
 *       the License, or (at your option) any later version.
 *
 *       This program is distributed in the hope that it will be useful,
 *       but WITHOUT ANY WARRANTY; without even the implied warranty of
 *       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 *       GNU General Public License for more details.
 *
 *       You should have received a copy of the GNU General Public
 *       License along with this program; see the file COPYING. If not,
 *       write to the Free Software Foundation, Inc., 59
 *       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ----------------------------------------------------------------------------
 */



#include <iomanip>

#include "tool/rw_tokenizer.h"
#include "tool/rw_cstring.h"
#include "tool/rw_locale.h"
#include "tool/config.h"

// #include <rw/vstream.h>
// #include <rw/rwfile.h>
#include "stat_tools.h"
#include "markovian.h"
#include "stat_label.h"


using namespace std;


extern int column_width(int nb_value , const double *value , double scale = 1.);
extern void cumul_computation(int nb_value , const double *pmass , double *pcumul);
extern void log_computation(int nb_value , const double *pmass , double *plog);



/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Chain.
 *
 *  arguments : type, nombre d'etats, flag initialisation.
 *
 *--------------------------------------------------------------*/

Chain::Chain(char itype , int inb_state , bool init_flag)

{
  type = itype;
  nb_state = inb_state;

  accessibility = 0;

  nb_component = 0;
  component_nb_state = 0;
  component = 0;
  state_type = 0;

  if (nb_state == 0) {
    nb_row = 0;

    initial = 0;
    transition = 0;
  }

  else {
    register int i , j;
    double *pinitial , *ptransition;

    nb_row = nb_state;

    initial = new double[type == 'o' ? nb_state : nb_row];

    if (init_flag) {
      pinitial = initial;
      for (i = 0;i < (type == 'o' ? nb_state : nb_row);i++) {
        *pinitial++ = 0.;
      }
    }

    transition = new double*[nb_row];
    for (i = 0;i < nb_row;i++) {
      transition[i] = new double[nb_state];

      if (init_flag) {
        ptransition = transition[i];
        for (j = 0;j < nb_state;j++) {
          *ptransition++ = 0.;
        }
      }
    }
  }

  cumul_initial = 0;
  cumul_transition = 0;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Chain.
 *
 *  arguments : type, nombre d'etats, nombre de lignes de la matrice
 *              des probabilites de transition, flag initialisation.
 *
 *--------------------------------------------------------------*/

Chain::Chain(char itype , int inb_state , int inb_row , bool init_flag)

{
  register int i , j;
  double *pinitial , *ptransition;


  type = itype;
  nb_state = inb_state;
  nb_row = inb_row;

  accessibility = 0;

  nb_component = 0;
  component_nb_state = 0;
  component = 0;
  state_type = 0;

  initial = new double[type == 'o' ? nb_state : nb_row];

  if (init_flag) {
    pinitial = initial;
    for (i = 0;i < (type == 'o' ? nb_state : nb_row);i++) {
      *pinitial++ = 0.;
    }
  }

  transition = new double*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new double[nb_state];

    if (init_flag) {
      ptransition = transition[i];
      for (j = 0;j < nb_state;j++) {
        *ptransition++ = 0.;
      }
    }
  }

  cumul_initial = 0;
  cumul_transition = 0;
}


/*--------------------------------------------------------------*
 *
 *  Copie des parametres d'une chaine de Markov.
 *
 *  argument : reference sur un objet Chain.
 *
 *--------------------------------------------------------------*/

void Chain::parameter_copy(const Chain &chain)

{
  register int i , j;
  double *pinitial , *cinitial , *ptransition , *ctransition;


  pinitial = initial;
  cinitial = chain.initial;
  for (i = 0;i < (type == 'o' ? nb_state : nb_row);i++) {
    *pinitial++ = *cinitial++;
  }

  if (chain.cumul_initial) {
    pinitial = cumul_initial;
    cinitial = chain.cumul_initial;
    for (i = 0;i < (type == 'o' ? nb_state : nb_row);i++) {
      *pinitial++ = *cinitial++;
    }
  }

  for (i = 0;i < nb_row;i++) {
    ptransition = transition[i];
    ctransition = chain.transition[i];
    for (j = 0;j < nb_state;j++) {
      *ptransition++ = *ctransition++;
    }
  }

  if (chain.cumul_transition) {
    for (i = 0;i < nb_row;i++) {
      ptransition = cumul_transition[i];
      ctransition = chain.cumul_transition[i];
      for (j = 0;j < nb_state;j++) {
        *ptransition++ = *ctransition++;
      }
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  Copie d'un objet Chain.
 *
 *  argument : reference sur un objet Chain.
 *
 *--------------------------------------------------------------*/

void Chain::copy(const Chain &chain)

{
  register int i , j;


  type = chain.type;
  nb_state = chain.nb_state;
  nb_row = chain.nb_row;

  if (chain.nb_component > 0) {
    accessibility = new bool*[nb_state];
    for (i = 0;i < nb_state;i++) {
      accessibility[i] = new bool[nb_state];
      for (j = 0;j < nb_state;j++) {
        accessibility[i][j] = chain.accessibility[i][j];
      }
    }

    nb_component = chain.nb_component;
    component_nb_state = new int[nb_component];
    component = new int*[nb_component];

    for (i = 0;i < nb_component;i++) {
      component_nb_state[i] = chain.component_nb_state[i];
      component[i] = new int[component_nb_state[i]];
      for (j = 0;j < component_nb_state[i];j++) {
        component[i][j] = chain.component[i][j];
      }
    }

    state_type = new char[nb_state];
    for (i = 0;i < nb_state;i++) {
      state_type[i] = chain.state_type[i];
    }
  }

  else {
    accessibility = 0;
    nb_component = 0;
    component_nb_state = 0;
    component = 0;
    state_type = 0;
  }

  initial = new double[type == 'o' ? nb_state : nb_row];

  if (chain.cumul_initial) {
    cumul_initial = new double[type == 'o' ? nb_state : nb_row];
  }
  else {
    cumul_initial = 0;
  }

  transition = new double*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new double[nb_state];
  }

  if (chain.cumul_transition) {
    cumul_transition = new double*[nb_row];
    for (i = 0;i < nb_row;i++) {
      cumul_transition[i] = new double[nb_state];
    }
  }
  else {
    cumul_transition = 0;
  }

  parameter_copy(chain);
}


/*--------------------------------------------------------------*
 *
 *  Destruction des champs d'un objet Chain.
 *
 *--------------------------------------------------------------*/

void Chain::remove()

{
  register int i;


  if (accessibility) {
    for (i = 0;i < nb_state;i++) {
      delete [] accessibility[i];
    }
    delete [] accessibility;
  }

  delete [] component_nb_state;

  if (component) {
    for (i = 0;i < nb_component;i++) {
      delete [] component[i];
    }
    delete [] component;
  }

  delete [] state_type;

  delete [] initial;
  delete [] cumul_initial;

  if (transition) {
    for (i = 0;i < nb_row;i++) {
      delete [] transition[i];
    }
    delete [] transition;
  }

  if (cumul_transition) {
    for (i = 0;i < nb_row;i++) {
      delete [] cumul_transition[i];
    }
    delete [] cumul_transition;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe Chain.
 *
 *--------------------------------------------------------------*/

Chain::~Chain()

{
  remove();
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe Chain.
 *
 *  argument : reference sur un objet Chain.
 *
 *--------------------------------------------------------------*/

Chain& Chain::operator=(const Chain &chain)

{
  if (&chain != this) {
    remove();
    copy(chain);
  }

  return *this;
}


/*--------------------------------------------------------------*
 *
 *  Analyse du format d'un objet Chain.
 *
 *  arguments : reference sur un objet Format_error, stream,
 *              reference sur l'indice de la ligne lue, type du processus
 *              ('o' : ordinaire, 'e' : en equilibre).
 *
 *--------------------------------------------------------------*/

Chain* chain_parsing(Format_error &error , ifstream &in_file , int &line , char type)

{
  RWLocaleSnapshot locale("en");
  RWCString buffer , token;
  size_t position;
  bool status = true , lstatus , **logic_transition;
  register int i;
  int read_line , nb_state = 0;
  long value;
  double proba , cumul;
  Chain *chain;


  chain = 0;

  // analyse lignes definissant le nombre d'etats et l'ordre

  while (buffer.readLine(in_file , false)) {
    line++;

#   ifdef DEBUG
    cout << line << "  " << buffer << endl;
#   endif

    position = buffer.first('#');
    if (position != RW_NPOS) {
      buffer.remove(position);
    }
    i = 0;

    RWCTokenizer next(buffer);

    while (!((token = next()).isNull())) {
      switch (i) {

      // test valeur nombre d'etats

      case 0 : {
        lstatus = locale.stringToNum(token , &value);
        if (lstatus) {
          if ((value < 2) || (value > NB_STATE)) {
            lstatus = false;
          }
          else {
            nb_state = value;
          }
        }

        if (!lstatus) {
          status = false;
          error.update(STAT_parsing[STATP_NB_STATE] , line , i + 1);
        }
        break;
      }

      // test mot cle STATES

      case 1 : {
        if (token != STAT_word[STATW_STATES]) {
          status = false;
          error.correction_update(STAT_parsing[STATP_KEY_WORD] , STAT_word[STATW_STATES] , line , i + 1);
        }
        break;
      }
      }

      i++;
    }

    if (i > 0) {
      if (i != 2) {
        status = false;
        error.update(STAT_parsing[STATP_FORMAT] , line);
      }
      break;
    }
  }

  if (nb_state == 0) {
    status = false;
    error.update(STAT_parsing[STATP_FORMAT] , line);
  }

  if (status) {
    chain = new Chain(type , nb_state , nb_state , false);

    // analyse probabilites initiales / probabilites de transition

    read_line = 0;
    while (buffer.readLine(in_file , false)) {
      line++;

#     ifdef DEBUG
      cout << line << "  " << buffer << endl;
#     endif

      position = buffer.first('#');
      if (position != RW_NPOS) {
        buffer.remove(position);
      }
      i = 0;

      RWCTokenizer next(buffer);

      if ((read_line == 0) || ((type == 'o') && (read_line == 2))) {
        while (!((token = next()).isNull())) {

          // test mot cle INITIAL_PROBABILITIES / TRANSITION_PROBABILITIES

          if (i == 0) {
            if ((type == 'o') && (read_line == 0)) {
              if (token != STAT_word[STATW_INITIAL_PROBABILITIES]) {
                status = false;
                error.correction_update(STAT_parsing[STATP_KEY_WORD] , STAT_word[STATW_INITIAL_PROBABILITIES] , line);
              }
            }

            else {
              if (token != STAT_word[STATW_TRANSITION_PROBABILITIES]) {
                status = false;
                error.correction_update(STAT_parsing[STATP_KEY_WORD] , STAT_word[STATW_TRANSITION_PROBABILITIES] , line);
              }
            }
          }

          i++;
        }

        if (i > 0) {
          if (i != 1) {
            status = false;
            error.update(STAT_parsing[STATP_FORMAT] , line);
          }
        }
      }

      else {
        cumul = 0.;

        while (!((token = next()).isNull())) {
          if (i < nb_state) {
            lstatus = locale.stringToNum(token , &proba);
            if (lstatus) {
              if ((proba < 0.) || (proba > 1. - cumul + DOUBLE_ERROR)) {
                lstatus = false;
              }

              else {
                cumul += proba;

                if ((type == 'o') && (read_line == 1)) {
                  chain->initial[i] = proba;
                }
                else {
                  chain->transition[read_line - (type == 'o' ? 3 : 1)][i] = proba;
                }
              }
            }

            if (!lstatus) {
              status = false;
              if ((type == 'o') && (read_line == 1)) {
                error.update(STAT_parsing[STATP_INITIAL_PROBABILITY] , line , i + 1);
              }
              else {
                error.update(STAT_parsing[STATP_TRANSITION_PROBABILITY] , line , i + 1);
              }
            }
          }

          i++;
        }

        if (i > 0) {
          if (i != nb_state) {
            status = false;
            error.update(STAT_parsing[STATP_FORMAT] , line);
          }

          if (cumul < 1. - DOUBLE_ERROR) {
            status = false;
            error.update(STAT_parsing[STATP_PROBABILITY_SUM] , line);
          }
        }
      }

      if (i > 0) {
        read_line++;
        if (((type == 'o') && (read_line == chain->nb_row + 3)) ||
            ((type == 'e') && (read_line == chain->nb_row + 1))) {
          break;
        }
      }
    }

    if (status) {

      // test etats atteignables

      status = chain->connex_component_research(error);

      logic_transition = chain->logic_transition_computation();
      status = chain->connex_component_research(error , logic_transition);

      if (status) {

        // test irreductibilite dans le cas en equilibre

        chain->component_computation(logic_transition);
        if ((type == 'e') && (chain->nb_component > 1)) {
          status = false;
          error.correction_update(STAT_parsing[STATP_CHAIN_STRUCTURE] , STAT_parsing[STATP_IRREDUCIBLE]);
        }
      }

      for (i = 0;i < nb_state;i++) {
        delete [] logic_transition[i];
      }
      delete [] logic_transition;
    }

    if (!status) {
      delete chain;
      chain = 0;
    }
  }

  return chain;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Chain.
 *
 *  arguments : stream, flag fichier.
 *
 *--------------------------------------------------------------*/

ostream& Chain::ascii_print(ostream &os , bool file_flag) const

{
  register int i , j;
  int buff , width;
  long old_adjust;


  old_adjust = os.setf(ios::left , ios::adjustfield);

  os << "\n" << nb_state << " " << STAT_word[STATW_STATES] << endl;

  // calcul des largeurs des colonnes

  width = column_width(nb_state , initial);
  for (i = 0;i < nb_row;i++) {
    buff = column_width(nb_state , transition[i]);
    if (buff > width) {
      width = buff;
    }
  }
  width += ASCII_SPACE;

  os << "\n";

  switch (type) {

  case 'o' : {
    os << STAT_word[STATW_INITIAL_PROBABILITIES] << endl;
    break;
  }

  case 'e' : {
    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_STATIONARY_PROBABILITIES] << endl;
    break;
  }
  }

  if ((type == 'e') && (file_flag)) {
    os << "# ";
  }
  for (i = 0;i < nb_state;i++) {
    os << setw(width) << initial[i];
  }
  os << endl;

  os << "\n" << STAT_word[STATW_TRANSITION_PROBABILITIES] << endl;

  for (i = 0;i < nb_row;i++) {
    for (j = 0;j < nb_state;j++) {
      os << setw(width) << transition[i][j];
    }
    os << endl;
  }

  if (nb_component > 0) {
    for (i = 0;i < nb_component;i++) {
      os << "\n";
      if (file_flag) {
        os << "# ";
      }

      switch (state_type[component[i][0]]) {
      case 't' :
        os << STAT_label[STATL_TRANSIENT] << " ";
        break;
      default :
        os << STAT_label[STATL_RECURRENT] << " ";
        break;
      }
      os << STAT_label[STATL_CLASS] << ": " << STAT_label[component_nb_state[i] == 1 ? STATL_STATE : STATL_STATES];

      for (j = 0;j < component_nb_state[i];j++) {
        os << " " << component[i][j];
      }

      if (state_type[component[i][0]] == 'a') {
        os << " (" << STAT_label[STATL_ABSORBING] << " " << STAT_label[STATL_STATE] << ")";
      }
    }
    os << endl;
  }

  os.setf((FMTFLAGS)old_adjust , ios::adjustfield);

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Chain au format tableur.
 *
 *  argument : stream.
 *
 *--------------------------------------------------------------*/

ostream& Chain::spreadsheet_print(ostream &os) const

{
  register int i , j;


  os << "\n" << nb_state << "\t" << STAT_word[STATW_STATES] << endl;

  switch (type) {
  case 'o' :
    os << "\n" << STAT_word[STATW_INITIAL_PROBABILITIES] << endl;
    break;
  case 'e' :
    os << "\n" << STAT_label[STATL_STATIONARY_PROBABILITIES] << endl;
    break;
  }

  for (i = 0;i < nb_state;i++) {
    os << initial[i] << "\t";
  }
  os << endl;

  os << "\n" << STAT_word[STATW_TRANSITION_PROBABILITIES] << endl;

  for (i = 0;i < nb_row;i++) {
    for (j = 0;j < nb_state;j++) {
      os << transition[i][j] << "\t";
    }
    os << endl;
  }

  if (nb_component > 0) {
    for (i = 0;i < nb_component;i++) {
      switch (state_type[component[i][0]]) {
      case 't' :
        os << "\n"<< STAT_label[STATL_TRANSIENT] << " ";
        break;
      default :
        os << "\n" << STAT_label[STATL_RECURRENT] << " ";
        break;
      }
      os << STAT_label[STATL_CLASS] << "\t" << STAT_label[component_nb_state[i] == 1 ? STATL_STATE : STATL_STATES];

      for (j = 0;j < component_nb_state[i];j++) {
        os << "\t" << component[i][j];
      }

      if (state_type[component[i][0]] == 'a') {
        os << " (" << STAT_label[STATL_ABSORBING] << " " << STAT_label[STATL_STATE] << ")";
      }
    }
    os << endl;
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWspace Chain::binaryStoreSize() const

{
  RWspace size;


  size = sizeof(nb_state) + sizeof(nb_row) + sizeof(nb_component);

  if (nb_component > 0) {
    register int i;

    size += sizeof(**accessibility) * nb_state * nb_state +
            sizeof(*component_nb_state) * nb_component;

    for (i = 0;i < nb_component;i++) {
      size += sizeof(**component) * component_nb_state[i];
    }

    size += sizeof(*state_type) * nb_state;
  }

  size += sizeof(*initial) * nb_state + sizeof(**transition) * nb_row * nb_state;

  return size;
}


void Chain::restoreGuts(RWvistream &is)

{
  register int i , j;


  remove();

  is >> nb_state >> nb_row >> nb_component;

  if (nb_component > 0) {
    accessibility = new bool*[nb_state];
    for (i = 0;i < nb_state;i++) {
      accessibility[i] = new bool[nb_state];
      for (j = 0;j < nb_state;j++) {
        is >> accessibility[i][j];
      }
    }

    component_nb_state = new int[nb_component];
    for (i = 0;i < nb_component;i++) {
      is >> component_nb_state[i];
    }

    component = new int*[nb_component];
    for (i = 0;i < nb_component;i++) {
      component[i] = new int[component_nb_state[i]];
      for (j = 0;j < component_nb_state[i];j++) {
        is >> component[i][j];
      }
    }

    state_type = new char[nb_state];
    for (i = 0;i < nb_state;i++) {
      is >> state_type[i];
    }
  }

  initial = new double[nb_state];
  for (i = 0;i < nb_state;i++) {
    is >> initial[i];
  }

  transition = new double*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new double[nb_state];
    for (j = 0;j < nb_state;j++) {
      is >> transition[i][j];
    }
  }
}


void Chain::restoreGuts(RWFile &file)

{
  register int i;


  remove();

  file.Read(nb_state);
  file.Read(nb_row);
  file.Read(nb_component);

  if (nb_component > 0) {
    accessibility = new bool*[nb_state];
    for (i = 0;i < nb_state;i++) {
      accessibility[i] = new bool[nb_state];
      file.Read(accessibility[i] , nb_state);
    }

    component_nb_state = new int[nb_component];
    file.Read(component_nb_state , nb_component);

    component = new int*[nb_component];
    for (i = 0;i < nb_component;i++) {
      component[i] = new int[component_nb_state[i]];
      file.Read(component[i] , component_nb_state[i]);
    }

    state_type = new char[nb_state];
    file.Read(state_type , nb_state);
  }

  initial = new double[nb_state];
  file.Read(initial , nb_state);

  transition = new double*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new double[nb_state];
    file.Read(transition[i] , nb_state);
  }
}


void Chain::saveGuts(RWvostream &os) const

{
  register int i , j;


  os << nb_state << nb_row << nb_component;

  if (nb_component > 0) {
    for (i = 0;i < nb_state;i++) {
      for (j = 0;j < nb_state;j++) {
        os << accessibility[i][j];
      }
    }

    for (i = 0;i < nb_component;i++) {
      os << component_nb_state[i];
    }
    for (i = 0;i < nb_component;i++) {
      for (j = 0;j < component_nb_state[i];j++) {
        os << component[i][j];
      }
    }

    for (i = 0;i < nb_state;i++) {
      os << state_type[i];
    }
  }

  for (i = 0;i < nb_state;i++) {
    os << initial[i];
  }
  for (i = 0;i < nb_row;i++) {
    for (j = 0;j < nb_state;j++) {
      os << transition[i][j];
    }
  }
}


void Chain::saveGuts(RWFile &file) const

{
  register int i;


  file.Write(nb_state);
  file.Write(nb_row);
  file.Write(nb_component);

  if (nb_component > 0) {
    for (i = 0;i < nb_state;i++) {
      file.Write(accessibility[i] , nb_state);
    }

    file.Write(component_nb_state , nb_component);
    for (i = 0;i < nb_component;i++) {
      file.Write(component[i] , component_nb_state[i]);
    }

    file.Write(state_type , nb_state);
  }

  file.Write(initial , nb_state);
  for (i = 0;i < nb_row;i++) {
    file.Write(transition[i] , nb_state);
  }
} */


/*--------------------------------------------------------------*
 *
 *  Creation des champs de type "cumul" d'un objet Chain.
 *
 *--------------------------------------------------------------*/

void Chain::create_cumul()

{
  register int i;


  if (!cumul_initial) {
    cumul_initial = new double[type == 'o' ? nb_state : nb_row];
  }

  if (!cumul_transition) {
    cumul_transition = new double*[nb_row];
    for (i = 0;i < nb_row;i++) {
      cumul_transition[i] = new double[nb_state];
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  Destruction des champs de type "cumul" d'un objet Chain.
 *
 *--------------------------------------------------------------*/

void Chain::remove_cumul()

{
  register int i;


  if (cumul_initial) {
    delete [] cumul_initial;

    cumul_initial = 0;
  }

  if (cumul_transition) {
    for (i = 0;i < nb_row;i++) {
      delete [] cumul_transition[i];
    }
    delete [] cumul_transition;

    cumul_transition = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Calcul des fonctions de repartition.
 *
 *--------------------------------------------------------------*/

void Chain::cumul_computation()

{
  if ((cumul_initial) && (cumul_transition)) {
    register int i;


    ::cumul_computation((type == 'o' ? nb_state : nb_row) , initial , cumul_initial);

    for (i = 0;i < nb_row;i++) {
      ::cumul_computation(nb_state , transition[i] , cumul_transition[i]);
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  Transfomation en log des parametres d'une chaine de Markov.
 *
 *--------------------------------------------------------------*/

void Chain::log_computation()

{
  if ((cumul_initial) && (cumul_transition)) {
    register int i;


    ::log_computation((type == 'o' ? nb_state : nb_row) , initial , cumul_initial);

    for (i = 0;i < nb_row;i++) {
      ::log_computation(nb_state , transition[i] , cumul_transition[i]);
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe Chain_data.
 *
 *  argument : reference sur un objet Chain_data.
 *
 *--------------------------------------------------------------*/

Chain_data::Chain_data(const Chain_data &chain_data)

{
  copy(chain_data);
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWspace Chain_data::binaryStoreSize() const

{
  RWspace size = sizeof(nb_state) + sizeof(nb_row) + sizeof(*initial) * nb_state +
                 sizeof(**transition) * nb_row * nb_state;

  return size;
}


void Chain_data::restoreGuts(RWvistream &is)

{
  register int i , j;


  remove();

  is >> nb_state >> nb_row;

  initial = new int[nb_state];
  for (i = 0;i < nb_state;i++) {
    is >> initial[i];
  }

  transition = new int*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new int[nb_state];
    for (j = 0;j < nb_state;j++) {
      is >> transition[i][j];
    }
  }
}


void Chain_data::restoreGuts(RWFile &file)

{
  register int i;


  remove();

  file.Read(nb_state);
  file.Read(nb_row);

  initial = new int[nb_state];
  file.Read(initial , nb_state);

  transition = new int*[nb_row];
  for (i = 0;i < nb_row;i++) {
    transition[i] = new int[nb_state];
    file.Read(transition[i] , nb_state);
  }
}


void Chain_data::saveGuts(RWvostream &os) const

{
  register int i , j;


  os << nb_state << nb_row;

  for (i = 0;i < nb_state;i++) {
    os << initial[i];
  }
  for (i = 0;i < nb_row;i++) {
    for (j = 0;j < nb_state;j++) {
      os << transition[i][j];
    }
  }
}


void Chain_data::saveGuts(RWFile &file) const

{
  register int i;


  file.Write(nb_state);
  file.Write(nb_row);

  file.Write(initial , nb_state);
  for (i = 0;i < nb_row;i++) {
    file.Write(transition[i] , nb_state);
  }
} */
