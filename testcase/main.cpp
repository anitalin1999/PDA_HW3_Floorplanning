#include<iostream>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include <math.h>

//#define READFILEDEBUG

using namespace std;

class HardBlock{
public:
    int idx;
    int width;
    int height;
    int area;
    int rotate;g
    int x;
    int y;
    HardBlock(int idx, int w, int h, int a, int r){
        idx = idx;
        width = w;
        height = h;
        area = w*h;
        rotate = r;
    }
    pair<int,int> getPin() {
        if (rotate == 0)
            return make_pair((x+width)/2,(y+height)/2);
        else
            return make_pair((x+height)/2,(y+width)/2);
    }
};

int hardblockNum;
int terminalNum;
int totalArea = 0;
int Wfl;
int Hfl;
int deadRatio = 0.15;
vector<HardBlock * > hardblocks;

int main(int argc, char* argv[]){
    fstream file1, file2, file3;
    
    // * read hardblocks file 
    file1.open(argv[1]);    
    string buff;
    
    file1 >> buff >> buff >> hardblockNum;
    file1 >> buff >> buff >> terminalNum;

#ifdef READFILEDEBUG
    cout << hardblockNum << " " << terminalNum << endl;
#endif
    int blockidx;
    int x[4];
    int y[4];
    for(int i=0; i<hardblockNum; i++){
        file1 >> buff;
        blockidx = stoi(buff.substr(2, buff.size()));
        file1 >> buff >> buff;
        for(int j=0; j<4; j++){
            file1 >> buff;
            x[j] = stoi(buff.substr(1, buff.size()-1));
            file1 >> buff;
            y[j] = stoi(buff.substr(0, buff.size()-1));
        }
              // read hardblock name
#ifdef READFILEDEBUG
        cout << blockidx << endl;
        for(int j=0; j<4; j++){
            cout << x[j] << " " << y[j] << endl;
        }
#endif
        HardBlock * hardblock1 = new HardBlock(blockidx, x[2]-x[0], y[2]-y[0], 0);
        totalarea += hardblock1->area;
        hardblocks.push_back(hardblock1);
    }
    Wfl = Hfl = (int) floor(sqrt(totalarea*(1+deadRatio))) ;

#ifdef READFILEDEBUG    
    cout << "totalarea " << totalArea << endl;
    cout << "deadRatio " << deadRatio << endl;
    cout << "Wfl " << Wfl << endl;
#endif
    
    
    file2.open(argv[2]);    // read pins file

    file3.open(argv[3]);    // read nets location file
    return 0;
}