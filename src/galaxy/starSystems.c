/*
Copyright (C) 2015-2016 Parallel Realities

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

#include "starSystems.h"

static void loadMissions(StarSystem *starSystem);
static StarSystem *loadStarSystem(cJSON *starSystemJSON);

void initStarSystems(void)
{
	cJSON *root, *node;
	char *text;
	StarSystem *starSystem, *tail;
	
	tail = &game.starSystemHead;
	
	text = readFile(getFileLocation("data/galaxy/starSystems.json"));
	root = cJSON_Parse(text);
	
	for (node = cJSON_GetObjectItem(root, "starSystems")->child ; node != NULL ; node = node->next)
	{
		starSystem = loadStarSystem(node);
		tail->next = starSystem;
		tail = starSystem;
	}
	
	cJSON_Delete(root);
	free(text);
}

static StarSystem *loadStarSystem(cJSON *starSystemJSON)
{
	StarSystem *starSystem;
	
	starSystem = malloc(sizeof(StarSystem));
	memset(starSystem, 0, sizeof(StarSystem));
	
	STRNCPY(starSystem->name, cJSON_GetObjectItem(starSystemJSON, "name")->valuestring, MAX_NAME_LENGTH);
	starSystem->side = lookup(cJSON_GetObjectItem(starSystemJSON, "side")->valuestring);
	starSystem->x = cJSON_GetObjectItem(starSystemJSON, "x")->valueint;
	starSystem->y = cJSON_GetObjectItem(starSystemJSON, "y")->valueint;
	
	if (cJSON_GetObjectItem(starSystemJSON, "fallsToPandorans"))
	{
		starSystem->fallsToPandorans = cJSON_GetObjectItem(starSystemJSON, "fallsToPandorans")->valueint;
	}
	
	if (strcmp(starSystem->name, "Sol") == 0)
	{
		starSystem->isSol = 1;
	}
	
	starSystem->missionHead.completed = 1;
	starSystem->missionTail = &starSystem->missionHead;
	
	loadMissions(starSystem);
	
	starSystem->x *= 3;
	starSystem->y *= 3;
	
	return starSystem;
}

static void loadMissions(StarSystem *starSystem)
{
	int i, count;
	char name[MAX_NAME_LENGTH];
	char path[MAX_FILENAME_LENGTH];
	char **filenames;
	
	STRNCPY(name, starSystem->name, MAX_NAME_LENGTH);
	
	for (i = 0 ; name[i] ; i++)
	{
		name[i] = tolower(name[i]);
	}
	
	sprintf(path, "data/missions/%s", name);
	
	filenames = getFileList(path, &count);
	
	for (i = 0 ; i < count ; i++)
	{
		sprintf(path, "data/missions/%s/%s", name, filenames[i]);
		
		loadMissionMeta(path);
		
		free(filenames[i]);
	}
	
	free(filenames);
}

StarSystem *getStarSystem(char *name)
{
	StarSystem *starSystem;
	
	for (starSystem = game.starSystemHead.next ; starSystem != NULL ; starSystem = starSystem->next)
	{
		if (strcmp(starSystem->name, name) == 0)
		{
			return starSystem;
		}
	}
	
	return NULL;
}

void updateStarSystemMissions(void)
{
	StarSystem *starSystem;
	Mission *mission, *prev;
	
	game.completedMissions = game.totalMissions = game.availableMissions = 0;
	
	for (starSystem = game.starSystemHead.next ; starSystem != NULL ; starSystem = starSystem->next)
	{
		starSystem->completedMissions = starSystem->availableMissions = starSystem->totalMissions = 0;
		
		for (mission = starSystem->missionHead.next ; mission != NULL ; mission = mission->next)
		{
			starSystem->totalMissions++;
			
			if (mission->completed)
			{
				starSystem->completedMissions++;
			}
		}
		
		if (strcmp(starSystem->name, "Sol") != 0)
		{
			game.totalMissions += starSystem->totalMissions;
			game.completedMissions += starSystem->completedMissions;
		}
	}
	
	for (starSystem = game.starSystemHead.next ; starSystem != NULL ; starSystem = starSystem->next)
	{
		prev = &starSystem->missionHead;
		
		for (mission = starSystem->missionHead.next ; mission != NULL ; mission = mission->next)
		{
			mission->available = strcmp(starSystem->name, "Sol") == 0 || isMissionAvailable(mission, prev);
			
			if (mission->available)
			{
				starSystem->availableMissions++;
			}
			
			prev = mission;
		}
		
		if (strcmp(starSystem->name, "Sol") != 0)
		{
			game.availableMissions += starSystem->availableMissions;
		}
		
		sprintf(starSystem->description, "[ %s ]  [ Missions %d / %d ]", starSystem->name, starSystem->completedMissions, starSystem->availableMissions);
	}
}

void destroyStarSystems(void)
{
	StarSystem *starSystem;
	Mission *mission;
	
	while (game.starSystemHead.next)
	{
		starSystem = game.starSystemHead.next;
		
		while (starSystem->missionHead.next)
		{
			mission = starSystem->missionHead.next;
			starSystem->missionHead.next = mission->next;
			free(mission);
		}
		
		game.starSystemHead.next = starSystem->next;
		free(starSystem);
	}
}
