/*
 *  Copyright (c) 2011 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#include <string.h>

#include <iostream>
#include <sstream>
#include <string>

#include "manifest_model.h"
#include "media_group.h"
#include "media_interval.h"
#include "media.h"

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using adaptive_manifest::ManifestModel;
using adaptive_manifest::MediaGroup;
using adaptive_manifest::MediaInterval;
using adaptive_manifest::Media;

static const string VERSION_STRING = "1.0.2.0";

static void Usage() {
  cout << "Usage: adaptive_prototype_manifest <-mg [mg options]";
  cout << " <-m [m options]>... >... [-mi options] [-o output_file]" << endl;
  cout << endl;
  cout << "Main options:" << endl;
  cout << "-h                    show help" << endl;
  cout << "-v                    show version" << endl;
  cout << "-?                    show help" << endl;
  cout << endl;
  cout << "mg options:" << endl;
  cout << "-id <string>          id of the MediaGroup" << endl;
  cout << "-lang <string>        lang of the MediaGroup" << endl;
  cout << endl;
  cout << "m options:" << endl;
  cout << "-id <string>          id of the Media" << endl;
  cout << "-file <string>        filename of the Media" << endl;
  cout << endl;
  cout << "mi options:" << endl;
  cout << "-duration <double>    duration in seconds" << endl;
  cout << "-groups <string>      list of MediaGroups separated by :" << endl;
  cout << "-id <string>          id of the Media" << endl;
  cout << "-start <double>       start time in seconds" << endl;
}

static void ParseMediaGroupOptions(const string& option_list, MediaGroup* mg) {
  stringstream ss(option_list);
  string option;
  while(std::getline(ss, option, ',')) {
    stringstream namevalue(option);
    string name;
    string value;
    std::getline(namevalue, name, '=');
    std::getline(namevalue, value, '=');

    if (name == "id") {
      mg->id(value);
    } else if (name == "lang") {
      mg->lang(value);
    }
  }
}

static double StringToDouble(const std::string& s) {
   std::istringstream iss(s);
   double value(0.0);
   iss >> value;
   return value;
 }

static void ParseMediaIntervalOptions(const string& option_list,
                                      MediaInterval* mi) {
  stringstream ss(option_list);
  string option;
  while(std::getline(ss, option, ',')) {
    stringstream namevalue(option);
    string name;
    string value;
    std::getline(namevalue, name, '=');
    std::getline(namevalue, value, '=');

    if (name == "id") {
      mi->id(value);
    } else if (name == "duration") {
      // TODO: Use boost::lexical_cast
      mi->duration(StringToDouble(value));
    } else if (name == "start") {
      mi->start(StringToDouble(value));
    } else if (name == "groups") {
      stringstream mi_ss(value);
      string group_id;
      while(std::getline(mi_ss, group_id, ':')) {
        mi->AddMediaGroupID(group_id);
      }
    }
  }
}

static void ParseMediaOptions(const string& option_list, Media* m) {
  stringstream ss(option_list);
  string option;
  while(std::getline(ss, option, ',')) {
    stringstream namevalue(option);
    string name;
    string value;
    std::getline(namevalue, name, '=');
    std::getline(namevalue, value, '=');

    if (name == "id") {
      m->id(value);
    } else if (name == "file") {
      m->file(value);
    }
  }
}

static bool ParseMainCommandLine(int argc,
                                 char* argv[],
                                 ManifestModel& model) {
  if (argc < 2)
    return false;

  for (int i=1; i<argc; ++i) {
    if (!strcmp("-mg", argv[i])) {
      model.AddMediaGroup();
      MediaGroup* mg = model.CurrentMediaGroup();
      string option_list(argv[++i]);
      ParseMediaGroupOptions(option_list, mg);
    } else if (!strcmp("-m", argv[i])) {
      MediaGroup* mg = model.CurrentMediaGroup();
      if (mg) {
        mg->AddMedia();
        Media* media = mg->CurrentMedia();
        string option_list(argv[++i]);
        ParseMediaOptions(option_list, media);
      }
    } else if (!strcmp("-mi", argv[i])) {
      model.AddMediaInterval();
      MediaInterval* mi = model.CurrentMediaInterval();
      string option_list(argv[++i]);
      ParseMediaIntervalOptions(option_list, mi);
    } else if (!strcmp("-o", argv[i])) {
      model.output_filename(argv[++i]);
    } else if (!strcmp("-v", argv[i])) {
      cout << "version: " << VERSION_STRING << endl;
    } else if ( (!strcmp("-h", argv[i])) || (!strcmp("-?", argv[i])) ) {
      return false;
    }
  }

  return true;
}

int main(int argc, char* argv[]) {
  ManifestModel model;
  
  if (!ParseMainCommandLine(argc, argv, model)) {
    Usage();
    return -1;
  }

  if (!model.Init()) {
    cout << "Manifest Model Init() Failed." << endl;
    return -1;
  }

  if (!model.OutputPrototypeManifestFile()) {
    cout << "OutputPrototypeManifestFile() Failed." << endl;
    return -1;
  }

	return 0;
}
