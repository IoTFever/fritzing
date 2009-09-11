// This file was generated by qlalr - DO NOT EDIT!

/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-08 Fachhochschule Potsdam - http://fh-potsdam.de

Fritzing is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fritzing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fritzing.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************

$Revision$:
$Author$:
$Date$

********************************************************************/

#include <QDebug>
#include "svgpathparser.h"
#include "svgpathlexer.h"

SVGPathParser::SVGPathParser()
{
}

SVGPathParser::~SVGPathParser()
{
}

QVector<QVariant> & SVGPathParser::symStack() {
	return m_symStack;
}

void SVGPathParser::reallocateStack()
{
    int size = m_stateStack.size();
    if (size == 0)
        size = 128;
    else
        size <<= 1;

    m_stateStack.resize(size);
}

QString SVGPathParser::errorMessage() const
{
    return m_errorMessage;
}

QVariant SVGPathParser::result() const
{
    return m_result;
}

bool SVGPathParser::parse(SVGPathLexer *lexer)
{
  const int INITIAL_STATE = 0;

  int yytoken = -1;

  reallocateStack();

  m_tos = 0;
  m_stateStack[++m_tos] = INITIAL_STATE;

  while (true) {
      const int state = m_stateStack.at(m_tos);
      if (yytoken == -1 && - TERMINAL_COUNT != action_index [state])
        yytoken = lexer->lex();
      int act = t_action (state, yytoken);
      if (act == ACCEPT_STATE)
        return true;

      else if (act > 0) {
          if (++m_tos == m_stateStack.size())
            reallocateStack();
          m_stateStack[m_tos] = act;
          yytoken = -1;
      } else if (act < 0) {
          int r = - act - 1;

          m_tos -= rhs [r];
          act = m_stateStack.at(m_tos++);

          switch (r) {
 case 0: {
    qDebug() << " got path_data ";
} break;  case 2: {
    //qDebug() << " got moveto_drawto_command_groups ";
} break;  case 4: {
    //qDebug() << " got moveto_drawto_command_group ";
} break;  case 6: {
    //qDebug() << " got drawto_commands  ";
} break;  case 16: {
    //qDebug() << " got drawto_command  ";
} break;  case 17: {
    qDebug() << "							got moveto ";
} break;  case 20: {
    //qDebug() << " got moveto_argument_sequence ";
} break;  case 21: {
    qDebug() << "							got lineto ";
} break;  case 24: {
    //qDebug() << " got lineto_argument_sequence  ";
} break;  case 25: {
    qDebug() << "							got horizontal_lineto ";
} break;  case 28: {
    //qDebug() << " got horizontal_lineto_argument_sequence ";
} break;  case 29: {
    qDebug() << "							got vertical_lineto ";
} break;  case 32: {
    //qDebug() << " got vertical_lineto_argument_sequence ";
} break;  case 33: {
    qDebug() << "							got curveto ";
} break;  case 36: {
    //qDebug() << " got curveto_argument_sequence 3 ";
} break;  case 40: {
    //qDebug() << " got curveto_argument ";
} break;  case 41: {
    qDebug() << "							got smooth_curveto ";
} break;  case 44: {
    //qDebug() << " got smooth_curveto_argument_sequence 3 ";
} break;  case 46: {
    //qDebug() << " got smooth_curveto_argument  ";
} break;  case 47: {
    qDebug() << "							got quadratic_bezier_curveto ";
} break;  case 50: {
    //qDebug() << " got quadratic_bezier_curveto_argument ";
} break;  case 52: {
    //qDebug() << " got quadratic_bezier_curveto_argument ";
} break;  case 53: {
    qDebug() << "							got elliptical_arc ";
} break;  case 56: {
    //qDebug() << " got elliptical_arc_argument_sequence ";
} break;  case 57: {
    //qDebug() << " got elliptical_arc_argument ";
} break;  case 58: {
    qDebug() << "							got smooth_quadratic_bezier_curveto ";
} break;  case 61: {
    //qDebug() << " got smooth_quadratic_bezier_curveto_argument_sequence 3 ";
} break;  case 63: {
    //qDebug() << " got coordinate_pair ";
} break;  case 64: {
    //qDebug() << " got x coordinate ";
} break;  case 65: {
    //qDebug() << " got y coordinate ";
} break;  case 67: {
    //qDebug() << " got comma_wsp 3 ";
} break;  case 68: {
    //qDebug() << " got wspplus ";
} break;  
case 69: {
    //qDebug() << " got coordinate ";
    m_symStack.append(lexer->currentNumber());
} break; 
 
case 70: {
    //qDebug() << " got nonnegative_number ";
    //not presently checking this is non-negative
    m_symStack.append(lexer->currentNumber());
} break; 
 
case 71: {
    //qDebug() << " got number ";
    m_symStack.append(lexer->currentNumber());
} break; 
 
case 72: {
    //qDebug() << " got flag ";
    //not presently checking this is only 0 or 1
    m_symStack.append(lexer->currentNumber());
} break; 
 
case 73: {
    //qDebug() << "							got moveto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 74: {
    //qDebug() << "							got lineto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 75: {
    //qDebug() << "							got horizontal_lineto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 76: {
    //qDebug() << "							got vertical_lineto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 77: {
    //qDebug() << "							got curveto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 78: {
    //qDebug() << "							got smooth curveto command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 79: {
    //qDebug() << "							got quadratic_bezier_curveto_command command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 
case 80: {
    //qDebug() << "							got smooth_quadratic_bezier_curveto_command command ";
    m_symStack.append(lexer->currentCommand());
} break; 
 case 81: {
    //qDebug() << "							got elliptical_arc_command ";
    m_symStack.append(lexer->currentCommand());
} break;  case 82: {
    qDebug() << "							got closepath ";
    m_symStack.append(lexer->currentCommand());
} break;  case 83: {
    qDebug() << "							got fakeclosepath ";
} break; 
          } // switch

          m_stateStack[m_tos] = nt_action(act, lhs[r] - TERMINAL_COUNT);

      } else {
          int ers = state;
          int shifts = 0;
          int reduces = 0;
          int expected_tokens[3];
          for (int tk = 0; tk < TERMINAL_COUNT; ++tk) {
              int k = t_action(ers, tk);

              if (! k)
                continue;
              else if (k < 0)
                ++reduces;
              else if (spell[tk]) {
                  if (shifts < 3)
                    expected_tokens[shifts] = tk;
                  ++shifts;
              }
          }

          m_errorMessage.clear();
          if (shifts && shifts < 3) {
              bool first = true;

              for (int s = 0; s < shifts; ++s) {
                  if (first)
                    m_errorMessage += QLatin1String("Expected ");
                  else
                    m_errorMessage += QLatin1String(", ");

                  first = false;
                  m_errorMessage += QLatin1String("`");
                  m_errorMessage += QLatin1String(spell[expected_tokens[s]]);
                  m_errorMessage += QLatin1String("'");
              }
          }

          return false;
        }
    }

    return false;
}
