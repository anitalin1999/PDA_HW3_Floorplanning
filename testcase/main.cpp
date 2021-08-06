#include<iostream>
#include<fstream>
#include<vector>
#include<stdlib.h>

//#define DEBUG

using namespace std;

class HardBlock{
public:
    int idx;
    int width;
    int height;
    int rotate;
    HardBlock(int idx, int w, int h, int r){
        idx = idx;
        width = w;
        height = h;
        rotate = r;
    }
};

int hardblockNum;
int terminalNum;

vector<HardBlock * > hardblocks;

int main(int argc, char* argv[]){
    fstream file1, file2, file3;
    
    // * read hardblocks file 
    file1.open(argv[1]);    
    string buff;
    
    file1 >> buff >> buff >> hardblockNum;
    file1 >> buff >> buff >> terminalNum;

#ifdef DEBUG
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
#ifdef DEBUG
        cout << blockidx << endl;
        for(int j=0; j<4; j++){
            cout << x[j] << " " << y[j] << endl;
        }
#endif
        HardBlock * hardblock1 = new HardBlock(blockidx, x[2]-x[0], y[2]-y[0], 0);
        HardBlock * hardblock2 = new HardBlock(blockidx, y[2]-y[0], x[2]-x[0], 1);
        hardblocks.push_back(hardblock1);
        hardblocks.push_back(hardblock2);
        //cout << blockidx << " | " << x[2]-x[0] << " " << y[2]-y[0] << endl;
    }

    file2.open(argv[2]);    // read nets file

    file3.open(argv[3]);    // read pins location file
    return 0;
}