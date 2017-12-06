/*! \file ui.c
 *  \brief Handles user interaction with the program. Displays a world.
 */


#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "libhowderek/howderek.h"
#include "libhowderek/howderek_hashmap.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_kv.h"


/*!
 *
 *  1. Print to console the steps taken by the robots in the world. One by one.
 *
 * Load graph into 2 dimensional array.
 * iterate through 2 dimensional array, printing it out.
 *
 * */

void renderer(struct world* w) {

    // Not yet implemented.

    //Variables etc.
    int i,j;
    char** worldArray = malloc(sizeof(char)*100);
    *worldArray = malloc(sizeof(char)*100);

    //loads the world into a 2 dimensional array
    for(i = 0;i< 100;i++){
        for(j = 0; j< 100 ;j++){
            struct howderek_kv_iter iter;
            howderek_kv_fill_iter(w->graph->vertex_map, &iter);
            while (howderek_kv_iterate(w->graph->vertex_map, &iter)) {
            howderek_graph_display_vertex_info(iter.value, NULL);

            worldArray[i][j] = iter.value;

            }
        }
    }


    //printing the 2 dimensional array
    for(i = 0;i< 100;i++){
        for(j = 0; j< 100 ;j++){
            printf("%c",worldArray[i][j]);
        }
    }
    //w->graph.vertex_map->array[i]
    //howderek_graph_display(&(w->graph));

    //Clears space inbetween, if I am to believe what Derek says
    howderek_clear();

}

void build_world(struct world* w, size_t size)
{
    int x;
    int y;
    int i;
    direction_t dir;
    int tmpx;
    position_t currPos;
    position_t tmpPos1;
    position_t tmpPos2;
    char wChar;
    char eChar;
    struct howderek_graph_vertex* currVertex;
    char** rawChars= w->rawChars;
    struct howderek_array* queue;
    //Flooding algorithim
    for(i = 0; i < NUMBER_OF_ROBOTS; i++) {
        queue = howderek_array_create(size);
        howderek_array_push(queue, w->robots[1].pos);
        while (queue->count != 0) {
            currVertex = howderek_array_dequeue(queue);
            currPos.bits = currVertex->id;
            tmpPos1.bits = currPos.bits;
            tmpx = x;
            for(dir = N; dir <= NW; dir++) {
                tmpPos2 = world_line_from(tmpPos1, 1, dir);
                if(rawChars[tmpPos2.coordinates.x][tmpPos2.coordinates.y] == ' ')
                {
                    howderek_array_push(queue, world_add(w, tmpPos2));
                    rawChars[tmpPos2.coordinates.x][tmpPos2.coordinates.y] = '.';
                }
            }
        }
        howderek_array_destroy(queue, queue->size);
    }

}


struct world* load_world(FILE* file) {
    char c;
    int lineCount = 0;
    int maxCharCount = 0;
    int charCount = 0;
    int x = 0;
    int y = 0;
    position_t robotPositions[NUMBER_OF_ROBOTS];
    position_t goalPositions[NUMBER_OF_ROBOTS];
    for (c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {
           lineCount++;
           if(charCount > maxCharCount)
           {
               maxCharCount = charCount;
           }
           charCount = 0;
        }
        else {
            charCount++;
        }
    }
    char rawChars[maxCharCount][lineCount];
    rewind(file);
    for (c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {
           y++;
           x = 0;
        }
        else {
            *(*(rawChars + y) + x) = c;
            x++;
            if(c == 'S')
            {
                robotPositions[0].coordinates.x = x;
                robotPositions[0].coordinates.y = y;
            }
            if(c == 'F')
            {
                robotPositions[1].coordinates.x = x;
                robotPositions[1].coordinates.y = y;
            }
            if(c == 'E')
            {
                goalPositions[0].coordinates.x = x;
                goalPositions[0].coordinates.y = y;
            }
            if(c == 'L')
            {
                goalPositions[1].coordinates.x = x;
                goalPositions[1].coordinates.y = y;
            }
        }
    }
    struct world* w = world_let_there_be_light(lineCount * maxCharCount);
    w->height = lineCount;
    w->width = maxCharCount;
    w->rawChars = rawChars;
    w->robots[0].pos = world_add(w, robotPositions[0]);
    w->robots[1].pos = world_add(w, robotPositions[1]);
    w->robots[0].goal = world_add(w, goalPositions[0]);
    w->robots[1].goal = world_add(w, goalPositions[1]);
    build_world(w, lineCount * maxCharCount);
    return w;
}

