#include<iostream>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include <math.h>
#include <climits>

// #define InitBlockBUG
// #define READFILEDEBUG

#define H INT_MAX
#define V INT_MIN

using namespace std;

class HardBlock{
public:
    int idx;
    int width;
    int height;
    int area;
    int rotate;
    int x;
    int y;
    HardBlock(int index, int w, int h, int r){
        idx = index;
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

class Pin{
public: 
    int idx;
    int x;
    int y;
    Pin(int index, int pinx, int piny){
        idx = index;
        x = pinx;
        y = piny;
    }
};

class Net{  
public:
    int degree;
    vector<pair<int, int> > connect;        // first: 0->block 1->pin , second: block/pin's idx
    Net(int netdegree){
        degree = netdegree;
    }

};


int hardblockNum;
int terminalNum;
int totalArea = 0;
int Wfl;
int Hfl;
double deadRatio = 0.15;
vector<HardBlock * > hardblocks;
vector<Pin *> pins;
vector<Net *> nets;

void showNPE(const vector<int> &npe){
    for(int i=0; i<npe.size(); i++){
        if (npe[i] == V){
            cout << "V";
        }
        else if (npe[i] == H){
            cout << "H";
        }
        else{
            cout << npe[i];
        }
    }
	cout << endl;
}

//we put block left to right, and if outline happen, we put block next level, and so on. 
//like 12V34VH56VH78VH
void npeInitial(vector<int> &npe) {
	npe.clear();
	int row_width = 0, x_cnt = 0, y_cnt = 0, cut_cnt = 0;
	for(int i = 0; i < hardblockNum; i ++) {
		row_width += hardblocks[i]->width;
		if(row_width >= Wfl) {
			y_cnt++;
			if(y_cnt >= 2) {
				npe.push_back(H);
				cut_cnt++;
				y_cnt = 1;
			}
			row_width = hardblocks[i]->width;
			x_cnt = 0;
		}
		npe.push_back(hardblocks[i]->idx);
		x_cnt++;
		if(x_cnt >= 2) {
			npe.push_back(V);
			cut_cnt++;
			x_cnt = 1;
		}
	}
	while(cut_cnt < hardblockNum - 1) {
		npe.push_back(H);
		cut_cnt++;
	}
#ifdef InitBlockBUG   
	showNPE(npe);
#endif
}

int main(int argc, char* argv[]){
    fstream file1, file2, file3;

    /* Read hardblocks file */
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

#ifdef READFILEDEBUG
        cout << blockidx << endl;
        for(int j=0; j<4; j++){
            cout << x[j] << " " << y[j] << endl;
        }
#endif
        HardBlock * hardblock1 = new HardBlock(blockidx, x[2]-x[0], y[2]-y[0], 0);
        totalArea += hardblock1->area;
        hardblocks.push_back(hardblock1);
    }
    
    /* Set fixed outline */
    Wfl = Hfl = (int) floor(sqrt(totalArea*(1+deadRatio))) ;

    vector<int> npe;    
    npeInitial(npe);

#ifdef READFILEDEBUG    
    cout << "totalarea " << totalArea << endl;
    cout << "deadRatio " << deadRatio << endl;
    cout << "Wfl " << Wfl << endl;
#endif
    
    /* Read pins file */
    file2.open(argv[2]); 
    int pinidx;
    int pinx, piny;   
    for(int i=0; i<terminalNum; i++){
        file2 >> buff;
        pinidx = stoi(buff.substr(1, buff.size()));
        file2 >> pinx >> piny;
        Pin * nowpin = new Pin(pinidx, pinx, piny);
        pins.push_back(nowpin);
    }

    /* Read nets file */
    file3.open(argv[3]);    
    int netNum;
    int netdegree;
    file3 >> buff >> buff;
    file3 >> netNum;
    file3 >> buff >> buff >> buff;
    for(int i=0; i<netNum; i++){
        file3 >> buff >> buff;
        file3 >> netdegree;
        Net * nownet = new Net(netdegree);
        for(int j=0; j<netdegree; j++){
            file3 >> buff;
            if(buff[0] == 's'){
                nownet->connect.push_back(make_pair(0, stoi(buff.substr(2, buff.size()))));
            }
            else{
                nownet->connect.push_back(make_pair(1, stoi(buff.substr(1, buff.size()))));
            }
        }

    #ifdef READFILEDEBUG
        cout << nownet->degree << " | ";
        for(vector<pair<int, int> >::iterator it=nownet->connect.begin(); it!=nownet->connect.end(); ++it){
            if(it->first == 0) cout << "sb";
            else cout << "p";
            cout << it->second << " ";
        }
        cout << endl;
    #endif

        nets.push_back(nownet);
    }   

    

    return 0;
}