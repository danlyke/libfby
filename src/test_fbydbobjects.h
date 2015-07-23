#ifndef TEST_FBYDBOBJECTS_H_INCLUDED
#define TEST_FBYDBOBJECTS_H_INCLUDED

#include "fbydb.h"
#include <string>
#include <float.h>

FBYCLASSPTR(TestNonCreatedTable);
FBYCLASS(TestNonCreatedTable) : public FbyORM /* nosql */ {
    FBYORM_SQLOBJECT;
public:
StatusUpdate() : FbyORM(BASEOBJINIT(StatusUpdate)),
        id(),
        status(),
        entered(),
        locationset(),
        latitude(),
        longitude()
        {
        }
       
       };

FBYCLASSPTR(TestCreatedTable);
FBYCLASS(TestCreatedTable) : public FbyORM /* nosql */ {
    FBYORM_SQLOBJECT;
public:
StatusUpdate() : FbyORM(BASEOBJINIT(StatusUpdate)),
        id(),
        status(),
        entered(),
        locationset(),
        latitude(),
        longitude(),
        twitter_updated(),
        facebook_updated(),
        flutterbynetlog_updated(),
        posaccuracy(),
        twitter_update(),
        facebook_update(),
        person_id(),
        person(),
        flutterby_update(),
        flutterby_updated(),
        identica_update(),
        identica_updated(),
        imagename(),
        thumbnailpath(),
        thumbnailwidth(),
        thumbnailheight(),
        uniquifer(),
        xid(),
        publishdelay(),
        twitterid()
        {}
    int id; // SQL INTEGER PRIMARY KEY,
    std::string status; // SQL TEXT
    time_t entered; // SQL TIMESTAMP,
    bool locationset; // SQL BOOLEAN
    double latitude; // SQL double precision             
    double longitude; // SQL double precision             
    bool twitter_updated; // SQL boolean                      default false
    bool facebook_updated; // SQL boolean                      default false
    bool flutterbynetlog_updated; // SQL boolean                      default false
    double posaccuracy; // SQL double precision             default 0
    bool twitter_update; // SQL boolean                      default false
    bool facebook_update; // SQL boolean                      default false
    int person_id; // SQL integer                      
    std::string person; // SQL TEXT
    bool flutterby_update; // SQL boolean                      default false
    bool flutterby_updated; // SQL boolean                      default false
    bool identica_update; // SQL boolean                      default false
    bool identica_updated; // SQL boolean                      default false
    std::string imagename; // SQL text                         
    std::string thumbnailpath; // SQL text                         
    int thumbnailwidth; // SQL integer                      
    int thumbnailheight; // SQL integer                      
    int uniquifer; // SQL integer                      default (-1)
    std::string xid; // SQL text                         
    double publishdelay; // SQL double precision             default 0
    std::string twitterid;
};


#endif /* #ifndef TEST_FBYDBOBJECTS_H_INCLUDED */
