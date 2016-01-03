#include <tinyxml2.h>
#include <iostream>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#include <gdal/ogr_api.h>
#include <gdal/ogr_geometry.h>
#include <float.h>
#include "fbydb.h"

using namespace std;
using namespace tinyxml2;
using namespace Fby;
using namespace GeographicLib;



const char * GetElementValue(const XMLDocument &doc, const char **path)
{
    const XMLNode *node = doc.FirstChildElement(*path);
    const XMLElement *element(NULL);
    path++;
    
    while (*path && NULL != (node = node->FirstChildElement(*path))
           && (element = node->ToElement()))
    {
        path++;
    }
    return element ? element->GetText() : "";
}

const char * GetElementValue(const XMLElement *element, const char **path)
{
    const XMLNode *node = element->FirstChildElement(*path);
    path++;
    
    while (*path && NULL != (node = node->FirstChildElement(*path))
           && (element = node->ToElement()))
    {
//        cout << "Searching under " << element->Name() << " for " << *path << endl;
        path++;
    }
    return (node && element) ? element->GetText() : "";
}


const XMLElement * GetElement(const XMLDocument &doc, const char **path)
{
    const XMLNode *node = doc.FirstChildElement(*path);
    const XMLElement *element(NULL);
    path++;
    
    while (*path && NULL != (node = node->FirstChildElement(*path))
           && (element = node->ToElement()))
    {
//        cout << "Searching under " << element->Name() << " for " << *path << endl;
        path++;
    }
    return element;
}

const char * GetElementValue(const XMLElement *element, const char *path)
{
    const XMLNode *node = element->FirstChildElement(path);
    if (!node) return "";
    element = node->ToElement();
    return element ? element->GetText() : "";
}


void DumpNode(XMLNode *node, int depth = 0)
{
    while (node)
    {
        for (int i = 0; i < depth; ++i)
        {
            cout << "  ";
        }
        XMLElement *element = node->ToElement();
        if (element)
        {
            cout << "Element: " << element->Name();
            for (const XMLAttribute *attribute = element->FirstAttribute(); attribute != NULL; attribute = attribute->Next())
            {
                cout << ", " << attribute->Name() << ":'" << attribute->Value() << "'";
            }
            cout << endl;
            DumpNode(element->FirstChild(), depth + 1);
        }
        XMLText *text = node->ToText();
        if (text)
        {
            cout << "Value: " << text->Value() << endl;
        }
        
        XMLComment *comment = node->ToComment();
        if (comment)
        {
            cout << "Comment: " << comment->Value() << endl;
        }
        
        node = node->NextSibling();
    }
}

string CalcMPH(double distance, time_t endtime, time_t begintime)
{
    char ach[32];
    double diffSeconds = (endtime - begintime);
    double distanceInMiles = distance / 1609.34;
    double diffMinutes = diffSeconds / 60.0;
    snprintf(ach, sizeof(ach), "%.5f", distanceInMiles / (diffMinutes / 60.0));
    return string(ach);
}

string CalcMinutesPerMile(double distance, time_t endtime, time_t begintime)
{
    char ach[32];
    double diffSeconds = (endtime - begintime);
    double distanceInMiles = distance / 1609.34;
    double diffMinutes = diffSeconds / 60.0;
    double minutesPerMile = diffMinutes / distanceInMiles;
    int minutes = floor(minutesPerMile);
    int seconds = round(60 * (minutesPerMile - (double)minutes));
    snprintf(ach,sizeof(ach),"%d:%02d", minutes, seconds);
    return string(ach);
}

int main(int argc, char **argv)
{
    const char *time_path[] = { "gpx", "metadata", "time", NULL };
    const char *trk_path[] = { "gpx", "trk", NULL };
    time_t firsttime(0);
    time_t lasttime(0);
    bool needs_comma = false;
    double distance(0.0);
    const Geodesic& geod = Geodesic::WGS84();
    
    
    for (int arg = 1; arg < argc; ++arg)
    {
        XMLDocument doc;
        doc.LoadFile( argv[arg] );
//        XMLElement *element = doc.RootElement();
        
//        DumpNode(element);
//        cout << "Time is " << GetElementValue(doc, time_path) << endl;
        cout << '[' << endl;

        for (const XMLElement *nodeTrack = GetElement(doc, trk_path);
             nodeTrack; nodeTrack = nodeTrack->NextSiblingElement("trk"))
        {
            for (const XMLElement *nodeTrackseg = nodeTrack->FirstChildElement("trkseg");
                 nodeTrackseg; nodeTrackseg = nodeTrackseg->NextSiblingElement("trkseg"))
            {
                double prevlat(FLT_MAX);
                double prevlon(FLT_MAX);
                time_t prevtime(0);
                
                for (const XMLElement *nodeTrackpoint = nodeTrackseg->FirstChildElement("trkpt");
                     nodeTrackpoint;
                     nodeTrackpoint = nodeTrackpoint->NextSiblingElement("trkpt"))
                {
                    double lat(FLT_MAX);
                    double lon(FLT_MAX);
                    if ((XML_NO_ERROR == nodeTrackpoint->QueryDoubleAttribute("lat", &lat))
                        && (XML_NO_ERROR == nodeTrackpoint->QueryDoubleAttribute("lon", &lon)))
                    {
                        const char *ele = GetElementValue(nodeTrackpoint,"ele");
                        const char *time = GetElementValue(nodeTrackpoint,"time");

//                        cout << "Trackpoint: " << lat << ", " << lon << " Elevation: " << ele << " at "
//                             << time;

                        time_t thistime = TextDateToTime(time);
                        if (!firsttime)
                            firsttime = thistime;
                       
                        if (prevlat != FLT_MAX && prevlon != FLT_MAX)
                        {
                            double s12(0.0);
                            geod.Inverse(lat, lon, prevlat, prevlon, s12);
                            distance += s12;
                            if (needs_comma)
                                cout << ",";
                            needs_comma = true;
                            cout << "{ \"time\" : " << (thistime - firsttime) << "000," << endl
                                 << "  \"mph\" : " << CalcMPH(s12, thistime, prevtime) << "," << endl
                                 << "  \"min_per_mile\" : \"" << CalcMinutesPerMile(s12, thistime, prevtime) << "\"," << endl
                                 << "  \"lat\" : " << lat  << "," << endl
                                 << "  \"lon\" : " << lon  << "," << endl
                                 << "  \"ele\" : " << ele  << "" <<  endl
                                 << "}";
                        }

                        
                       
                        if (const XMLElement *nodeExtensions = nodeTrackpoint->FirstChildElement("extensions"))
                        {
//                            cout << " speed " << GetElementValue(nodeExtensions, "gpx10:speed");
//                            cout << " accuracy " << GetElementValue(nodeExtensions, "ogt10:accuracy");
//                            cout << " course " << GetElementValue(nodeExtensions, "gpx10:course");
                        }
                        cout << endl;
                        lasttime = thistime;
                        prevtime = thistime;
                        prevlat = lat;
                        prevlon = lon;
                    }
                }
            }
        }
        cout << "]" << endl;
    }
//    cout << "Distance " << distance << " speed " << CalcMPH(distance, lasttime, firsttime) << " min/mi " << CalcMinutesPerMile(distance, lasttime, firsttime) << endl;
}
