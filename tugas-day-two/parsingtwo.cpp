#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

int main(){

    int nomorMovie;
    cin >> nomorMovie;

    string moviePath = string(DATA_DIR) + "/motion_movie/" + to_string(nomorMovie) + ".json";
    
    ifstream fileMovie(moviePath);
    if (!fileMovie.is_open()){
        cout << "Gagal membuka file movie: " << moviePath << endl;
        return 1;
    }

    stringstream bufferMovie; 
    bufferMovie << fileMovie.rdbuf();
    string teksMovie = bufferMovie.str();

    Document docMovie; 
    docMovie.Parse(teksMovie.c_str());
    if(docMovie.HasParseError()){
        cout << "Format JSON movie salah!" << endl;
        return 1; 
    }

    const Value& motionUnits = docMovie["motion_unit"];
    int unitIdRefInt = motionUnits[0]["id"].GetInt();
    string unitIdRef = to_string(unitIdRefInt); 

    string unitPath = string(DATA_DIR) + "/motion_unit/" + unitIdRef + ".json";
    
    ifstream fileUnit(unitPath);
    if (!fileUnit.is_open()){
        cout << "Gagal membuka file unit: " << unitPath << endl;
        return 1;
    }

    stringstream bufferUnit;
    bufferUnit << fileUnit.rdbuf();
    string teksUnit = bufferUnit.str();

    Document docUnit;
    docUnit.Parse(teksUnit.c_str());
    if(docUnit.HasParseError()){
        cout << "Format JSON unit salah!" << endl;
        return 1;
    }

    cout << "File ID: " << unitIdRef << endl;
    cout << "File Name: " << docUnit["name"].GetString() << endl; 
    cout << "Total Frame: " << docUnit["total_frame"].GetInt() << endl;

    
    const Value& times = docUnit["time"];
    const Value& motionFrames = docUnit["motion_frame"];
    int totalFrame = docUnit["total_frame"].GetInt();

    
    for (int i = 0; i < totalFrame; i++) {
        cout << "\nFrame ke-" << i << " | Waktu (Time): " << times[i].GetInt() << endl;
        cout << "  Motion Frame Data: ";
        

        const Value& singleFrame = motionFrames[i];
        for (SizeType j = 0; j < singleFrame.Size(); j++) {
            cout << singleFrame[j].GetInt() << " ";
        }
    }
    cout << endl;

    return 0;


}