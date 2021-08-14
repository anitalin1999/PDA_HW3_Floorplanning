#include<iostream>
#include<fstream>
#include<vector>
#include <stack>
#include<stdlib.h>
#include <math.h>
#include <climits>
#include <algorithm>
#include <iomanip>

#define PerturbBUG
// #define WireLengthBUG
// #define blockAllocateBUG
// #define AreaCostBUG
// #define BulidRecordBUG
// #define BulidTreeBUG
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
    int pinX;
    int pinY;
    HardBlock(int index, int w, int h, int r){
        idx = index;
        width = w;
        height = h;
        area = w*h;
        rotate = r;
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

class Node
{
public:
    int idx;
    int x,y;                                //leftcorner for blocks like 34V56VH and so no
    vector<vector<int> > record;            //width,height,lChoice,rChoice;
    Node *lNode, *rNode, *parent;
    Node(int index)
    {
        idx = index;
        lNode = NULL;
        rNode = NULL;
        parent = NULL;
    }
};



int hardblockNum;
int terminalNum;
int totalArea = 0;
int Wfl;
int Hfl;
int rootChoice;
double deadRatio = 0.15;
vector<HardBlock * > hardblocks;
vector<Pin *> pins;
vector<Net *> nets;
Node* treeRoot;

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

// postorder traversal
void showTree(Node* root)
{
    if (!root) return;
    showTree(root->lNode);
    showTree(root->rNode);
    if (root->idx == V){
        cout << "V ";
#ifdef BulidRecordBUG
        cout << endl;
        for (int i = 0; i < root->record.size(); i++)
        {
            cout << "(" << root->record[i][0] << "," << root->record[i][1] << ") ";
        }
        cout << endl;
#endif        
    }
    else if (root->idx == H){
        cout << "H ";
#ifdef BulidRecordBUG
        cout << endl;
        for (int i = 0; i < root->record.size(); i++)
        {
            cout << "(" << root->record[i][0] << "," << root->record[i][1] << ") ";
        }
        cout << endl;
#endif                
    }
    else{
        cout << root->idx << " ";
#ifdef BulidRecordBUG
        cout << endl;
        for (int i = 0; i < root->record.size(); i++)
        {
            cout << "(" << root->record[i][0] << "," << root->record[i][1] << ") ";
        }
        cout << endl;
#endif                
    }   
}

//high to low
bool cmp(const vector<int> &a, const vector<int> &b) {
	return a[1] > b[1];
}

//buttom-up
void bulidRecord(Node* node)
{
	node -> record.clear();    
    if (node->idx == V){
 	    Node *lchild = node -> lNode, *rchild = node -> rNode;
		int l = 0, r = 0;
		while(l < lchild -> record.size() && r < rchild -> record.size()) {
			vector<int> row;
			row.push_back(lchild -> record[l][0] + rchild -> record[r][0]);
			row.push_back(max(lchild -> record[l][1], rchild -> record[r][1]));
			row.push_back(l);
			row.push_back(r);
			node -> record.push_back(row);
			// choose block-shape merge i++ j++
			if(lchild -> record[l][1] > rchild -> record[r][1])
				l++;
			else if(lchild -> record[l][1] < rchild -> record[r][1])
				r++;
			else {
				l++;
				r++;
			}
		}
        sort(node -> record.begin(), node -> record.end(), cmp);                
    }else if (node->idx == H){
 	    Node *lchild = node -> lNode, *rchild = node -> rNode;        
		int l = lchild -> record.size() - 1, r = rchild -> record.size() - 1;
		while(l >= 0 && r >= 0) {
			vector<int> row;
			row.push_back(max(lchild -> record[l][0], rchild -> record[r][0]));
			row.push_back(lchild -> record[l][1] + rchild -> record[r][1]);
			row.push_back(l);
			row.push_back(r);
			node -> record.push_back(row);
			if(lchild -> record[l][0] > rchild -> record[r][0])
				l--;
			else if(lchild -> record[l][0] < rchild -> record[r][0])
				r--;
			else {
				l--;
				r--;
			}
		}
    }else{
        int height = hardblocks[node->idx]->height, width = hardblocks[node->idx]->width;
        vector<int> row1,row2;
        if (height > width){         
            row1.push_back(width);
            row1.push_back(height);
            row2.push_back(height);
            row2.push_back(width);
            node -> record.push_back(row1);
            node -> record.push_back(row2);
        }else if (height < width){
            row1.push_back(width);
            row1.push_back(height);
            row2.push_back(height);
            row2.push_back(width);
            node -> record.push_back(row2);
            node -> record.push_back(row1);            
        }else{
            row1.push_back(width);
            row1.push_back(height);
            node -> record.push_back(row1);                     
        }
    }    
    
    
}

void npeBuildTree(vector<int> &npe)
{
	stack<Node*> s;
	Node *lchild, *rchild;
	for(int i = 0; i < npe.size(); i++) {
		if(npe[i] != V && npe[i] != H){
			s.push(new Node(npe[i]));
            bulidRecord(s.top());
        }
		else {
			rchild = s.top();
			s.pop();
			lchild = s.top();
			s.pop();
			s.push(new Node(npe[i]));
			rchild -> parent = s.top();
			lchild -> parent = s.top();
			s.top() -> rNode = rchild;
			s.top() -> lNode = lchild;
            bulidRecord(s.top());
		}        
	}    
    treeRoot = s.top();
    treeRoot->x = 0;
    treeRoot->y = 0;
#if defined BulidTreeBUG || defined BulidRecordBUG
    s.pop();
    if (!s.empty()){
        cout << "stack is not empty.\n"; 
    }
	showTree(treeRoot);
    cout << endl;
#endif    
}

void blockAllocate(int choice, Node* node) {           
    if (node != NULL)
    {
        if (node->idx == V) {
            int lNodeChoice = node->record[choice][2];
            int rNodeChoice = node->record[choice][3];
            node->lNode->x = node->x;
            node->lNode->y = node->y;
            node->rNode->x = node->x + node->lNode->record[lNodeChoice][0];
            node->rNode->y = node->y;
            blockAllocate(lNodeChoice,node->lNode);
            blockAllocate(rNodeChoice,node->rNode);
        } else if (node->idx == H) {      
            int lNodeChoice = node->record[choice][2];
            int rNodeChoice = node->record[choice][3];            
            node->lNode->x = node->x;
            node->lNode->y = node->y;
            node->rNode->x = node->x; 
            node->rNode->y = node->y + node->lNode->record[lNodeChoice][1];
            blockAllocate(lNodeChoice,node->lNode);
            blockAllocate(rNodeChoice,node->rNode);
        } else {
            int width = hardblocks[node->idx]->width;
            int height = hardblocks[node->idx]->height;
            if(width == node->record[choice][0]){
                hardblocks[node->idx]->rotate = 0;
                hardblocks[node->idx]->pinX = (node->x+width)/2;
                hardblocks[node->idx]->pinY = (node->y+height)/2;
            }else{
                hardblocks[node->idx]->rotate = 1;
                hardblocks[node->idx]->pinX = (node->x+height)/2;
                hardblocks[node->idx]->pinY = (node->y+width)/2;
            }
            hardblocks[node->idx]->x = node->x;
            hardblocks[node->idx]->y = node->y;
#ifdef blockAllocateBUG
            cout << setw(8) << node->idx;
            cout << setw(8) << hardblocks[node->idx]->x;
            cout << setw(8) << hardblocks[node->idx]->y;
            cout << setw(8) << hardblocks[node->idx]->width;
            cout << setw(8) << hardblocks[node->idx]->height;                   
            cout << setw(8) << hardblocks[node->idx]->rotate;    
            cout << setw(8) << hardblocks[node->idx]->pinX;
            cout << setw(8) << hardblocks[node->idx]->pinY;
            cout << endl;
#endif            
        }
    }
}

int npeAreaCost(Node* node) {
	int area, minArea = INT_MAX;
    int width,height;
#ifdef AreaCostBUG
    cout << "Hfl " << Hfl << endl;
    cout << "Wfl " << Wfl << endl;
#endif
	for(int i = 0; i < node -> record.size(); i++) {        
        width = node -> record[i][0];
        height = node -> record[i][1];
		if(width > Wfl && height > Hfl)
			area = width * height - Wfl * Hfl;
		else if(width > Wfl && height <= Hfl)
			area = (width - Wfl) * height;
		else if(width <= Wfl && height > Hfl)
			area = width * (height - Hfl);
		else
			area = 0;
#ifdef AreaCostBUG
        cout << "width " << width << " height " << height << " outlineArea " << area << endl;
#endif            
		if(minArea > area) {
			minArea = area;            
			rootChoice = i;
		}
	}
#ifdef AreaCostBUG
    cout << "minArea " << minArea << endl;
    cout << "choice " << rootChoice << endl;
#endif
	return minArea;
}

void drawFloorplan(){
    ofstream floorplanFile;
    floorplanFile.open ("../draw/draw.floorplan");
    /*
    int outlineW = 0;
    int outlineH = 0;
    for(int i=0; i<hardblocks.size(); i++){
        if(hardblocks[i]->rotate == 0){
            if(hardblocks[i]->x + hardblocks[i]->width > outlineW) outlineW = hardblocks[i]->x + hardblocks[i]->width;
            if(hardblocks[i]->y + hardblocks[i]->height > outlineH) outlineH = hardblocks[i]->y + hardblocks[i]->height;
        }
        else{
            if(hardblocks[i]->x + hardblocks[i]->height > outlineW) outlineW = hardblocks[i]->x + hardblocks[i]->height;
            if(hardblocks[i]->y + hardblocks[i]->width > outlineH) outlineH = hardblocks[i]->y + hardblocks[i]->width;
        }
    }
    floorplanFile << "0\t" << "0\t" << outlineW << "\t" << outlineH << endl;
    */
    for(int i=0; i<hardblocks.size(); i++){
        floorplanFile << hardblocks[i]->x << "\t";
        floorplanFile << hardblocks[i]->y << "\t";
        if(hardblocks[i]->rotate == 0){
            floorplanFile << hardblocks[i]->width << "\t";
            floorplanFile << hardblocks[i]->height << endl;
        }
        else{
            floorplanFile << hardblocks[i]->height << "\t";
            floorplanFile << hardblocks[i]->width << endl;
        }
        
        
    }
    floorplanFile.close();
}
    
int npeWireLength() {
	int wl = 0;
	for(int i = 0; i < nets.size(); i++) {
		int x_min = INT_MAX, x_max = INT_MIN, y_min = INT_MAX, y_max = INT_MIN;
		for(int j = 0; j < nets[i]->degree; j++) {
            int type = nets[i]->connect[j].first, index = nets[i]->connect[j].second;
#ifdef WireLengthBUG            
            cout << setw(8) << index;
#endif            
            if (type == 0) {
#ifdef WireLengthBUG                
                cout << setw(8) << hardblocks[index]->idx;
                cout << setw(8) << hardblocks[index]->pinX;
                cout << setw(8) << hardblocks[index]->pinY << endl;
#endif                
			    x_min = min(x_min, hardblocks[index]->pinX);
			    x_max = max(x_max, hardblocks[index]->pinX);
			    y_min = min(y_min, hardblocks[index]->pinY);
			    y_max = max(y_max, hardblocks[index]->pinY);
            }
            else {
#ifdef WireLengthBUG                
                cout << setw(8) << pins[index - 1]->idx;
                cout << setw(8) << pins[index - 1]->x;
                cout << setw(8) << pins[index - 1]->y << endl;
#endif                
			    x_min = min(x_min, pins[index - 1]->x);
			    x_max = max(x_max, pins[index - 1]->x);
			    y_min = min(y_min, pins[index - 1]->y);
			    y_max = max(y_max, pins[index - 1]->y);   
            }
		}
#ifdef WireLengthBUG
        cout << "x_max " << x_max << " x_min " << x_min;
        cout << " y_max " << y_max << " y_min " << y_min << endl;
#endif        
		wl += (x_max - x_min) + (y_max - y_min);
	}
	return wl;
}

bool npeBallot(vector<int> &npe, int p){

    return true;
}

bool npeSkew(vector<int> &npe, int p){
    
    return true;
}


void npePerturb(vector<int> &npe, int m){
    // Swap two operands
    if(m == 0){
        int operandsArray[npe.size()];
        int operandsCnt = 0;
        int b1, b2;
        for(int i=0; i<npe.size(); i++){
            if(npe[i] != V && npe[i]!= H){
                operandsArray[operandsCnt] = i;         // npe[operansArray[operandsCnt]] has a block
                operandsCnt ++;
            }   
        }
        b1 = rand() % operandsCnt;
        b2 = rand() % operandsCnt;
        while(b1 == b2) b2 = rand() % operandsCnt;
        // Swap
        int empty = npe[operandsArray[b1]];
        npe[operandsArray[b1]] = npe[operandsArray[b2]];
        npe[operandsArray[b2]] = empty;
    }
    // complement a chain of operators
    else if(m == 1){
        int chainArray[npe.size()];
        int chainCnt = 0;
        int flag = 0;
        int c;
        for(int i=0; i<npe.size(); i++){
            if((npe[i] == H || npe[i] == V) && flag == 0){
                flag = 1;
                chainArray[chainCnt] = i;               // noe[chainArray[chainCnt]] has a chain head
                chainCnt ++;
            }
            else if((npe[i] != H || npe[i] != V) && flag == 1){
                flag = 0;
            }
        }
        c = rand() % chainCnt;
#ifdef PerturbBUG
        cout << "choose " << c << "th chain to complement." << endl;
#endif
        int chainIndex = chainArray[c];
        while(npe[chainIndex] == H || npe[chainIndex] == V){
            if(npe[chainIndex] == H) npe[chainIndex] = V;
            else npe[chainIndex] = H;
            chainIndex ++;
        }

    }
    // Swap two ADJACENT operand and opertor (check balloting and skewed property)
    else{
        int adjArray[npe.size()];
        int adjCnt = 0;
        for(int i=0; i<npe.size()-1; i++){
            if( ((npe[i] != H || npe[i] != V) && (npe[i+1] == H || npe[i+1] == V)) || ((npe[i] == H || npe[i] == V) && (npe[i+1] != H || npe[i+1] != V)) ){
                adjArray[adjCnt] = i;
                adjCnt++; 
            }
        }
        int adjIdx = rand() % adjCnt;
        while(!npeBallot(npe, adjArray[adjIdx]) && npeSkew(npe, adjArray[adjIdx]) ){
            adjIdx = rand() % adjCnt;
        }
        int empty = npe[adjArray[adjIdx]];
        npe[adjArray[adjIdx]]= npe[adjArray[adjIdx+1]];
        npe[adjArray[adjIdx+1]] = empty;
    }
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

    showNPE(npe);
    npePerturb(npe, 2);
    showNPE(npe);

    npeBuildTree(npe);
    npeAreaCost(treeRoot);    
#ifdef blockAllocateBUG
    showTree(treeRoot);
    cout << "blockAllocate" << endl;
    cout << setw(8); 
    cout << "index" << setw(8) << "x" << setw(8) << "y" << setw(8);
    cout << "width" << setw(8) << "height" << setw(8) << "rotate" << setw(8);
    cout << "pin x" << setw(8) << "pin y" << endl; 
#endif
    blockAllocate(rootChoice,treeRoot);
    
    /* Create "draw floorplan file" named draw.floorplan */
    drawFloorplan();

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

#ifdef WireLengthBUG
    cout << "WireLength" << endl; 
//R_index for index record in net 
//T_index for index record in pin or hardblock
//to check index is correct
    cout << setw(8) << "R_index" << setw(8) << "T_index";
    cout << setw(8) << "pin x" << setw(8) << "pin y" << endl; 
#endif
    npeWireLength();
    

    return 0;
}