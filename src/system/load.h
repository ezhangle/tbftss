/*
Copyright (C) 2015 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "SDL2/SDL.h"

#include "../defs.h"
#include "../structs.h"
#include "../json/cJSON.h"

extern char *readFile(char *filename);
extern StarSystem *getStarSystem(char *name);
extern Mission *getMission(char *filename);
extern Challenge *getChallenge(Mission *mission, int type);
extern int lookup(char *lookup);
extern char *getSaveFilePath(char *filename);
extern char *getLookupName(char *prefix, long num);

extern Game game;