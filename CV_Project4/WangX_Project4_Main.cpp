#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

class Property {
public:
	int label;
	int numP;
	int minR;
	int minC;
	int maxR;
	int maxC;
	
	
	Property(int l, int np, int nr, int nc, int mr, int mc) {
		label = l;
		numP = np;
		minR = nr;
		maxR = mr;
		minC = nc;
		maxC = mc;
	}
	Property(){}
};

class ccLabel {
public:
	int numR;
	int numC;
	int minV;
	int maxV;
	int newLabel;
	int trueNumCC;
	int newMin;
	int newMax;
	
	int NZNAry[6];
	int** ZFAry;
	int* EQAry;
	char option;
	Property* CCproperty;

	ccLabel(ifstream& in) {
		in >> numR >> numC>> minV >> maxV;
		newLabel = 0;

		ZFAry = new int* [numR+2];
		EQAry = new int[(numR * numC)/2];
		for (int i = 0; i < numR+2; i++) {
			ZFAry[i] = new int[numC+2];
		}

	}

	void zero2DAry(int** ary) {
		for (int i = 0; i < numR+2; i++) {
			for (int j = 0; j < numC+2; j++) {
				ary[i][j] = 0;
			}
		}
	}

	void negative1D() {
		for (int i = 0; i < 6; i++) {
			NZNAry[i] = -1;
		}
	}

	void loadImg(ifstream& in) {
		for (int i = 1; i < numR+1; i++) {
			for (int j = 1; j < numC+1; j++) {
				in >> ZFAry[i][j];
			}
		}
	}

	void conversion() {
		for (int i = 1; i < numR + 1; i++) {
			for (int j = 1; j < numC + 1; j++) {
				if(ZFAry[i][j] == 1)
					ZFAry[i][j] =0;
				else
					ZFAry[i][j] = 1;
			}
		}
	}

	void prettyPrint(int** inAry, ofstream& out) {
		//out << numR << " " << numC << " " << minV << " " << maxV << endl;
		string s = to_string(newLabel);

		int w = s.length();
		int r = 1;
		while (r < numR + 1) {
			int c = 1;
			while (c < numC + 1) {
				if (inAry[r][c] == 0) {
					out << ".";
				}
				else {
					out << inAry[r][c];
				}
				s = to_string(inAry[r][c]);
				int ww = s.length();
				while (ww <= w) {
					out << " ";
					ww++;
				}
				c++;
			}
			out << endl;
			r++;
		}
	}

	void printImg(int** inAry, ofstream& out) {
		out << numR << " " << numC << " " << minV << " " << maxV << endl;
		string s = to_string(newLabel);

		int w = s.length();
		int r = 1;
		while (r < numR + 1) {
			int c = 1;
			while (c < numC + 1) {
				out << inAry[r][c];

				s = to_string(inAry[r][c]);
				int ww = s.length();
				while (ww <= w) {
					out << " ";
					ww++;
				}
				c++;
			}
			out << endl;
			r++;
		}
	}

	int checkNeighbor8(int i, int j, int c) {
		negative1D();
		int n = 0;
		if(c == 1)
		{
			int x = -1;
			int y = -1;
			while (x < 1) {
				y = -1;
				while (y < 2 && !(x==0 && y>=0)) {
					if (ZFAry[i + x][j + y] > 0) {
						NZNAry[n] = ZFAry[i + x][j + y];
						n++;
					}
					y++;
				}
				x++;
			}
		}
		if (c == 2)
		{
			int x = 0;
			int y = -1;
			while(x < 2){
				if (x == 0)
					y = 0;
				else
					y = -1;
				while(y < 2){
					if (ZFAry[i + x][j + y] > 0) {
						NZNAry[n] = ZFAry[i + x][j + y];
						n++;
					}
					y++;
				}
				x++;
			}
		}
		int e = 0;
		for (int x = 1; x < n; x++) {
			if (NZNAry[x] == NZNAry[x - 1])
				e++;
		}
		NZNAry[5] = e;

		sort(NZNAry, NZNAry + n);
		return n;
	}

	int checkNeighbor4(int i, int j, int c) {
		negative1D();
		int n = 0;
		if (c == 1) {
			if(ZFAry[i + -1][j + 0] > 0)
			{
				NZNAry[n] = ZFAry[i + -1][j + 0];
				n++;
			}
			if (ZFAry[i + 0][j + -1] > 0)
			{
				NZNAry[n] = ZFAry[i + 0][j + -1];
				n++;
			}

			if (NZNAry[0] == NZNAry[1]  && n == 2 || n == 1)
				NZNAry[5] = 1;
		}

		if (c == 2) {
			if (ZFAry[i + 0][j + 1] > 0)
			{
				NZNAry[n] = ZFAry[i + 0][j + 1];
				n++;
			}
			if (ZFAry[i + 1][j + 0] > 0)
			{
				NZNAry[n] = ZFAry[i + 1][j + 0];
				n++;
			}
			NZNAry[n] = ZFAry[i][j];
			n++;
			if (NZNAry[0] == NZNAry[1] && NZNAry[2] == NZNAry[1] && n == 2 || NZNAry[0] == ZFAry[i][j] && n == 1)
				NZNAry[5] = 1;
		}
		
		sort(NZNAry, NZNAry + n);
		return n;
	}

	void connect8Pass1() {
		newLabel = 0;
		for (int i = 1; i < numR + 1; i++) {
			for (int j = 1; j < numC + 1; j++) {
				if (ZFAry[i][j] > 0) {
					int n = checkNeighbor8(i, j, 1);
					if (n == 0) {
						newLabel++;
						ZFAry[i][j] = newLabel;
						EQAry[ZFAry[i][j]] = newLabel;
					}
					else if (n == (NZNAry[5] + 1) || n==1) {
						ZFAry[i][j] = EQAry[NZNAry[0]];
					}
					else {
						ZFAry[i][j] = NZNAry[0];
						updateEQ(n);
					}
				}
			}
		}
	}

	void connect8Pass2() {
		for (int i = numR+1; i > 0; i--) {
			for (int j = numC+1 ; j > 0; j--) {
				if (ZFAry[i][j] > 0) {
					int n = checkNeighbor8(i, j, 2);
					if (n == 0) {
					}
					else if (n == (NZNAry[5] - 1)) {
					}
					else {
						int minL = EQAry[NZNAry[0]];
						if(ZFAry[i][j] > minL)
						{
							EQAry[ZFAry[i][j]] = minL;
							ZFAry[i][j] = minL;
							updateEQ(n);
						}
					}
					ZFAry[i][j] = EQAry[ZFAry[i][j]];
				}
			}
		}
	}

	void connect4Pass1() {
		newLabel = 0;
		for (int i = 1; i < numR + 1; i++) {
			for (int j = 1; j < numC + 1; j++) {
				if (ZFAry[i][j] > 0) {
					int n = checkNeighbor4(i, j, 1);
					if (n == 0) {
						newLabel++;
						ZFAry[i][j] = newLabel;
						EQAry[ZFAry[i][j]] = newLabel;
					}
					else if (1 == NZNAry[5]) {
						ZFAry[i][j] = EQAry[NZNAry[0]];
					}
					else {
						ZFAry[i][j] = EQAry[NZNAry[0]];
						updateEQ(n);
					}
				}
			}
		}
	}

	void connect4Pass2() {
		for (int i = numR + 1; i > 0; i--) {
			for (int j = numC + 1; j > 0; j--) {
				if (ZFAry[i][j] > 0) {
					int n = checkNeighbor4(i, j, 2);
					if (n == 0) {
					}
					else if (1 == (NZNAry[5])) {
					}
					else {
						int minL = NZNAry[0];
						if (ZFAry[i][j] > minL)
						{
							EQAry[ZFAry[i][j]] = minL;
							ZFAry[i][j] = minL;
							updateEQ(n);
						}
					}
					ZFAry[i][j] = EQAry[ZFAry[i][j]];
				}
			}
		}
	}

	void updateEQ(int n) {
		for (int i = 0; i < n; i++) {
			if (EQAry[NZNAry[i]] > EQAry[NZNAry[0]])
				EQAry[NZNAry[i]] = EQAry[NZNAry[0]];
		}
	}

	int manageEQAry() {
		int rd = 0;
		int i = 1;
		while (i <= newLabel)
		{
			if (i != EQAry[i])
			{
				EQAry[i] = EQAry[EQAry[i]];
			}
			else {
				rd++;
				EQAry[i] = rd;
			}
			i++;
		}
		return rd;
	}
	void connected4(ofstream& o, ofstream& debug) {
		debug << "entering connected4 method \n";
		connect4Pass1();
		debug << "After connected4 pass1, newLabel = " << newLabel << endl;
		o << "Result of: Pass 1\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table after Pass 1:  ";
		printEQAry(o);

		connect4Pass2();
		debug << "After connected4 pass2, newLabel = " << newLabel << endl;
		o << "\nResult of: Pass 2\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table after Pass 2:  ";
		printEQAry(o);

		trueNumCC = manageEQAry();
		o << "\nEquivalency Table after EQ Table Management:  ";
		printEQAry(o);
		newMin = 0;
		newMax = trueNumCC;
		CCproperty = new Property[trueNumCC + 1];
		debug << "In connected4, after manage EQAry, trueNumCC = " << trueNumCC << endl;

		connectPass3(CCproperty, trueNumCC, debug);
		o << "\nResult of: Pass 3\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table after Pass 3:  ";
		printEQAry(o);
		debug << "Leaving connected4 method \n";
	}

	void connected8( ofstream& o, ofstream& debug) {
		debug << "entering connected8 method \n";
		connect8Pass1();
		debug << "After connected8 pass1, newLabel = " << newLabel << endl;
		o << "Result of: Pass 1\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table after Pass 1: ";
		printEQAry(o);

		connect8Pass2();
		debug << "After connected8 pass2, newLabel = " << newLabel << endl;
		o << "\nResult of: Pass 2\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table afterPass 2:  ";
		printEQAry(o);

		trueNumCC = manageEQAry();
		o << "\nEquivalency Table after EQ Table Management:  ";
		printEQAry(o);
		newMin = 0;
		newMax = trueNumCC;
		CCproperty = new Property[trueNumCC + 1];
		debug << "In connected8, after manage EQAry, trueNumCC = " << trueNumCC << endl;

		connectPass3(CCproperty, trueNumCC, debug);
		o << "\nResult of: Pass 3\n";
		prettyPrint(ZFAry, o);
		o << "\nEquivalency Table after Pass 3:  ";
		printEQAry(o);
		debug << "Leaving connected8 method \n";
	}

	void connectPass3(Property* cp, int tc, ofstream& debug) {
		debug << "entering connectPass3 method \n";
		for (int i = 1; i < trueNumCC + 1; i++) {
			CCproperty[i] = Property(i, 0, numR, numC, 0, 0);
		}

		for (int r = 1; r < numR + 1; r++) {
			for (int c = 1; c < numC + 1; c++) {
				if (ZFAry[r][c] > 0) {
					ZFAry[r][c] = EQAry[ZFAry[r][c]];
					int k = ZFAry[r][c];
					CCproperty[k].numP++;
					if (r < CCproperty[k].minR)
						CCproperty[k].minR = r - 1;
					if (r > CCproperty[k].maxR)
						CCproperty[k].maxR = r - 1;
					if (c <= CCproperty[k].minC)
						CCproperty[k].minC = c - 1;
					if (c > CCproperty[k].maxC)
						CCproperty[k].maxC = c - 1;
				}
			}
		}
		debug << "leaving connectPass3 method \n";
	}


	void printCCproperty(ofstream& o) {
		o << numR << " " << numC << " " << minV << " " << maxV << endl;
		o << trueNumCC << endl;
		for (int i = 1; i < trueNumCC + 1; i++) {
			o << CCproperty[i].label << endl;
			o << CCproperty[i].numP << endl;
			o << CCproperty[i].minR << "\t" << CCproperty[i].minC << endl;
			o << CCproperty[i].maxR << "\t" << CCproperty[i].maxC << endl;
		}
	}


	void drawBoxes(Property* cp, int tc, ofstream& debug) {
		debug << "entering drawBoxes method" << endl;
		int index = 1;

		while (index <= trueNumCC) {
			int minR = CCproperty[index].minR + 1;
			int minC = CCproperty[index].minC + 1;
			int maxR = CCproperty[index].maxR + 1;
			int maxC = CCproperty[index].maxC + 1;
			int label = CCproperty[index].label;

			for (int i = minC; i <= maxC; i++) {
				ZFAry[minR][i] = label;
				ZFAry[maxR][i] = label;
			}
			for (int i = minR; i <= maxR; i++) {
				ZFAry[i][minC] = label;
				ZFAry[i][maxC] = label;
			}
			index++;
		}
		debug << "leaving drawBoxes method" << endl;
	}

	void printEQAry(ofstream& o) {
		o << "(indexing starts from 1)" << endl;
		for (int i = 1; i <= newLabel; i++) {
			o << i << " ";
		}
		o << endl;
		for (int i = 1; i <= newLabel; i++) {
			o << EQAry[i] << " ";
			if (i >= 10 && EQAry[i]<10)
				o << " ";
		}
		o << endl;
	}
	
};

int main(int argc, char* argv[]) {
	ifstream in(argv[1]);
	string Connectness = (argv[2]);
	string option = (argv[3]);
	ofstream RFprettyPrintFile(argv[4]);
	ofstream labelFile(argv[5]);
	ofstream propertyFile(argv[6]);
	ofstream debug(argv[7]);
	int cn = stoi(Connectness);

	ccLabel c = ccLabel(in);
	c.zero2DAry(c.ZFAry);
	c.loadImg(in);

	if (option == "Y" || option == "y") 
		c.conversion();

	if (cn == 4)
		c.connected4(RFprettyPrintFile, debug);

	if(cn == 8)
		c.connected8(RFprettyPrintFile, debug);

	c.printImg(c.ZFAry, labelFile);
	c.printCCproperty(propertyFile);
	c.drawBoxes(c.CCproperty, c.trueNumCC, debug);
	RFprettyPrintFile << "\nNumber of Connected Components: " << c.trueNumCC << endl;
	RFprettyPrintFile << "\nBounding Boxes: " << endl;
	c.prettyPrint(c.ZFAry, RFprettyPrintFile);
	

	labelFile.close();
	propertyFile.close();
	RFprettyPrintFile.close();
	debug.close();
	in.close();
}