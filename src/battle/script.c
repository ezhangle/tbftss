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

#include "script.h"

static void executeNextLine(ScriptRunner *runner);

static cJSON *scriptJSON;
static ScriptRunner head;
static ScriptRunner *tail;
static char funcNameBuffer[MAX_NAME_LENGTH];

void initScript(cJSON *scriptNode)
{
	memset(&head, 0, sizeof(ScriptRunner));
	tail = &head;
	
	scriptJSON = scriptNode;
}

void doScript(void)
{
	ScriptRunner *runner, *prev;
	
	prev = &head;
	
	for (runner = head.next ; runner != NULL ; runner = runner->next)
	{
		runner->delay = MAX(0, runner->delay - 1);
		
		if (!runner->delay)
		{
			executeNextLine(runner);
			
			if (!runner->line)
			{
				if (runner == tail)
				{
					tail = prev;
				}
				
				prev->next = runner->next;
				free(runner);
				runner = prev;
			}
		}
		
		prev = runner;
	}
}

void runScriptFunction(const char *format, ...)
{
	ScriptRunner *scriptRunner;
	cJSON *function;
	char *functionName;
	va_list args;

	memset(&funcNameBuffer, '\0', sizeof(funcNameBuffer));

	va_start(args, format);
	vsprintf(funcNameBuffer, format, args);
	va_end(args);
	
	function = scriptJSON->child;
	
	while (function)
	{
		functionName = cJSON_GetObjectItem(function, "function")->valuestring;
		
		if (strcmp(functionName, funcNameBuffer) == 0)
		{
			scriptRunner = malloc(sizeof(ScriptRunner));
			memset(scriptRunner, 0, sizeof(ScriptRunner));
			
			scriptRunner->line = cJSON_GetObjectItem(function, "lines")->child;
			
			tail->next = scriptRunner;
			tail = scriptRunner;
			
			printf("Running script '%s'\n", funcNameBuffer);
		}
		
		function = function->next;
	}
}

static void executeNextLine(ScriptRunner *runner)
{
	char *line;
	char command[24];
	char strParam[2][256];
	int intParam[2];

	line = runner->line->valuestring;
	
	sscanf(line, "%s", command);

	if (strcmp(command, "ACTIVATE_ENTITY") == 0)
	{
		sscanf(line, "%*s %[^\n]", strParam[0]);
		activateEntities(strParam[0]);
	}
	else if (strcmp(command, "ACTIVATE_OBJECTIVE") == 0)
	{
		sscanf(line, "%*s %d", &intParam[0]);
		activateObjective(intParam[0]);
	}
	else if (strcmp(command, "MSG_BOX") == 0)
	{
		sscanf(line, "%*s %d %[^;]%*c%[^\n]", &intParam[0], strParam[0], strParam[1]);
		addMessageBox(intParam[0], strParam[0], strParam[1]);
	}
	else if (strcmp(command, "WAIT") == 0)
	{
		sscanf(line, "%*s %d", &intParam[0]);
		runner->delay = intParam[0] * FPS;
	}
	else if (strcmp(command, "COMPLETE_MISSION") == 0)
	{
		addHudMessage(colors.green, "Mission Complete!");
		completeMission();
	}
	else if (strcmp(command, "FAIL_MISSION") == 0)
	{
		addHudMessage(colors.red, "Mission Failed!");
		failMission();
	}
	else if (strcmp(command, "RETREAT_ALLIES") == 0)
	{
		battle.epic = 0;
		retreatAllies();
	}
	else if (strcmp(command, "RETREAT_ENEMIES") == 0)
	{
		battle.epic = 0;
		retreatEnemies();
	}
	else
	{
		printf("ERROR: Unrecognised script command '%s'\n", command);
		printf("ERROR: Offending line: '%s'\n", line);
		exit(1);
	}

	runner->line = runner->line->next;
}

void destroyScript(void)
{
	ScriptRunner *scriptRunner;
	
	while (head.next)
	{
		scriptRunner = head.next;
		head.next = scriptRunner->next;
		free(scriptRunner);
	}
	
	tail = &head;
}