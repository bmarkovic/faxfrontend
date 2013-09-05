/***************************************************************************
$Id: faxstat.h,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          faxstat.h - Front End for Hylafax
                             -------------------
    begin                : Fri Apr 18 2003
    copyright            : (C) 2003 by Glenn Burkhardt
    email                : gbburkhardt@verizon.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FAXSTAT_H
#define FAXSTAT_H
#include "faxstatDsgn.h"

class FaxStat : public FaxStatDsgn
{ 
    Q_OBJECT

public:
    FaxStat();
    ~FaxStat();

protected:
    void timerEvent(QTimerEvent*);
};

#endif // FAXSTAT_H
