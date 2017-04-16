#include <iostream>
#include <cstdlib>
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <ctime>
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
using namespace std;

int indexOfA = 65;
int numberOfGames = 51;
int numberOfReels = 5;
string delimiter = " ";
ifstream file;
string line;
int gains;
int losts;
int matches;
string symbol;
string nextSymbol;
int numberOfRunsPerGame;
int numberOfSymbols;
int reelSize;
int numberOfBettingLines; // Numero de lineas apostadas
int numberOfPaylines; // Numero total de lineas ganadoras
int wildGain[5];
int scatterGain[5];
int* bettingLines; // Arreglo indicando cuales lineas se estan apostando
int* indeces;
int** gainOfSymbols;
int** payLines; // Lineas ganadoras
char** reels;
int seed;
string seedFileName;

int totalGains;

void WriteTextToLogFile(string inputString){
    /* Imprime string introducido, en el archivo log.
        inputString: String de entrada.*/
    std::ofstream outfile;
    outfile.open("Output.log", std::ios_base::app);
    outfile << inputString;
}

void WriteEnterToLogFile(){
    /* Ingresa un "enter" para moverse a la siguiente linea en el archivo log.*/
    std::ofstream outfile;
    outfile.open("Output.log", std::ios_base::app);
    outfile << "" << endl;
}

void PrintTime(){
    //Imprime el tiempo actual en el archivo log.
    WriteTextToLogFile("*****************************");
    WriteEnterToLogFile();
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(buffer);
    WriteTextToLogFile(buffer);
    WriteEnterToLogFile();
    WriteEnterToLogFile();
}

void PrintGameTitle(int pGameNumber){
    /* Imprime el titulo para cada juego en el archivo log.
    pGameNumber: Numero de Juego.
    */
    WriteTextToLogFile("*****************************");
    WriteEnterToLogFile();
    WriteTextToLogFile("********** GAME " + SSTR (pGameNumber) + " ***********"); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    WriteEnterToLogFile();
    WriteEnterToLogFile();
}

void PrintPlayNumber(int pPlayNumber){
    /*Imprime el numero de juego en el archivo log.
        pPlayNumber: numero de juego actual.
    */
    WriteTextToLogFile ("Play #: " + SSTR( pPlayNumber )); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    WriteEnterToLogFile();
}

void PrintIndexes(int pIndexes[]){
    /*Imprime los indices por jugada en el log.
        pIndexes: arreglo de indices aleatorios para la corrida de simbolos.
    */
    WriteTextToLogFile("Indexes: ");
    for(int x=0; x<5; x++){
        WriteTextToLogFile (" " + SSTR( pIndexes[x] ) + " "); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    }
    WriteEnterToLogFile();
    WriteEnterToLogFile();
}

void PrintSymbols(){
    //Imprime la corrida de simbolos en el archivo log.
    WriteTextToLogFile("Symbols: ");
    WriteEnterToLogFile();
    for (int j=0;j<3; j++){
        for(int i=0; i<5; i++){
            WriteTextToLogFile(SSTR(reels[i][(indeces[i]+j)%reelSize] ) + " "); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
        }
        WriteEnterToLogFile();
    }
    WriteEnterToLogFile();
    WriteEnterToLogFile();
}

void PrintPayline(int** pPaylines, int pPaylinesRow){
    /*Imprime la linea de pago ganadora en el archivo log.
        pPaylines: matriz de lineas de pago.
        pPaylinesRow: fila ganadora.
    */
    for (int i = 0; i<5; i++){
        WriteTextToLogFile(SSTR(pPaylines[pPaylinesRow][i]) + " "); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    }
}

void PrintWinningSymbol(){
    //Imprime el simbolo ganador en el archivo log.
    if(matches > 1){
        WriteTextToLogFile (": " + SSTR(matches+1) + " " + SSTR(symbol) + "'s :    "); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
        } else {
        WriteTextToLogFile (": " + SSTR(matches+1) + " " + SSTR(symbol) + "   :    ");
        }
}

void PrintTotalGains(int pTotalGains){
    /* Imprime la cantidad total ganada por juego.
    pTotalWinnings: Cantidad total ganada por juego.
    */
    WriteTextToLogFile ("Total winnings on this play: " + SSTR( pTotalGains )); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    WriteEnterToLogFile();
    WriteEnterToLogFile();
}

void PrintScatters(int pScatterCount, int pScatterGain){
    /*Imprime en el archivo los scatters.
        pScatterCount: cantidad de scatters en el juego
        pScatterGain: monto ganado con los scatters
    */
    if(pScatterCount > 1){
        WriteTextToLogFile ("          : " + SSTR (pScatterCount) + " $'s :    " + SSTR (pScatterGain)); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    } else {
        WriteTextToLogFile ("          : " + SSTR (pScatterCount) + " $   :    " + SSTR (pScatterGain));
    }
}

void PrintSymbolGains(int pSymbolGains){
    /* Imprime la cantidad ganada por simbolo.
    pSymbolGains: Cantidad ganada por simbolo.
    */
    WriteTextToLogFile(SSTR(pSymbolGains)); // SSTR: metodo definido al inicio de main.cpp, el cual convierte a string.
    WriteEnterToLogFile();
}

/* Tokenizer
 *
 * Recibe un indicador numerico "globalArray" referenciando a cual arreglo va a insertar dato
 * Recibe a cual posicion "i" en especifico le va a insertar
 * Recibe la cantidad "c" de valores a insertar
 * Recibe un string "line" el cual separa por espacios " " para obtener los diferentes valores
 */
void Tokenizer(string line,int globalArray,int i,int c){
    size_t pos = 0;
    string token;
    int counter=0;
    while (counter<c) {
        pos = line.find(delimiter);
        token = line.substr(0, pos);
        switch(globalArray){
            case 1:
                gainOfSymbols[i][counter] = atoi(token.c_str());
                break;
            case 2:
                wildGain[counter] = atoi(token.c_str());
                break;
            case 3:
                scatterGain[counter] = atoi(token.c_str());
                break;
            case 4:
                reels[i][counter] = token[0];
                break;
            case 5:
                payLines[i][counter] = atoi(token.c_str());
                break;
            case 6:
                bettingLines[counter] = atoi(token.c_str());
        }
        line.erase(0,pos+delimiter.length());
        counter++;
    }
}

/* SetMachine
 *
 * Recibe la direccion del archivo de configuracion de la maquina "fileName"
 * Y carga los datos correspondientes
 */
void SetMachine(string fileName){
    file.open(fileName.c_str());

    if (file.is_open()){
        //<# de simbolos : entero positivo>
        getline(file,line);
        numberOfSymbols=atoi(line.c_str());

        //0 <ganancia 2> <ganancia 3> <ganancia 4> <ganancia 5>
        //... /* # de simbolos veces; reales positivos */
        //0 <ganancia 2> <ganancia 3> <ganancia 4> <ganancia 5>

        gainOfSymbols=(int**) malloc(sizeof(int*) * numberOfSymbols);
        for(int i=0;i<numberOfSymbols;i++){
            gainOfSymbols[i]=(int*) malloc(sizeof(int) * numberOfReels);
            getline(file,line);
            Tokenizer(line,1,i,5);
        }

        //0 <wild 2> <wild 3> <wild 4> <wild 5>
        getline(file,line);
        Tokenizer(line,2,0,5);

        //<scatter 1> <scatter 2> <scatter 3> <scatter 4> <scatter 5>
        getline(file,line);
        Tokenizer(line,3,0,5);

        //<reel size : entero positivo>
        getline(file,line);
        reelSize=atoi(line.c_str());

        //<configuracion rueda 1>
        //<configuracion rueda 2>
        //<configuracion rueda 3>
        //<configuracion rueda 4>
        //<configuracion rueda 5>
        reels=(char**) malloc(sizeof(char*) * numberOfReels);
        for(int i=0;i<5;i++){
            reels[i]=(char*) malloc(sizeof(char) * reelSize);
            getline(file,line);
            Tokenizer(line,4,i,reelSize);
        }

        //<# paylines : entero positivo>
        getline(file,line);
        numberOfPaylines = atoi(line.c_str());

        //<reel 1> <reel 2> <reel 3> <reel 4> <reel 5>
        //... /* # paylines veces, de 1 a 3 */
        //<reel 1> <reel 2> <reel 3> <reel 4> <reel 5>
        payLines=(int**) malloc(sizeof(int*) * numberOfPaylines);
        for(int i=0;i<numberOfPaylines;i++){
            payLines[i]=(int*) malloc(sizeof(int) * numberOfReels);
            getline(file,line);
            Tokenizer(line,5,i,5);
        }

        file.close();
    }
    else cout << "Error al abrir el archivo "<<fileName;
}

/* SetGame
 *
 * Inicializa algunas variables
 * toma el input de jugadas y lineas apostadas
 */
void SetGame(string gameFile){
    indeces = (int*) malloc(sizeof(int) * numberOfReels);
    for (int k = 0; k < numberOfReels; ++k) {
        indeces[k] = 0;
    }

    file.open(gameFile.c_str());
    if (file.is_open()){
        getline(file,line);
        numberOfRunsPerGame = atoi(line.c_str());

        getline(file,line);
        numberOfBettingLines = atoi(line.c_str());

        bettingLines = (int*) malloc(sizeof(int) * numberOfBettingLines);
        if(numberOfBettingLines == numberOfPaylines){
            for (int i = 0; i < numberOfBettingLines; ++i) {
                bettingLines[i] = i;
            }
        }else{
            getline(file,line);
            Tokenizer(line,6,0,numberOfBettingLines);
        }

        file.close();
    }else cout << "Error al abrir el archivo "<<gameFile;


    //SetRandomSeed();
}

/* GetNumberOfScatters
 *
 * Returns the numbers of Scatters of the machine
 */
int GetNumberOfScatters(){
    int numberOfScatters = 0;
    for (int j = 0; j < numberOfReels; ++j){
        symbol = reels[j][indeces[j]];
        if (symbol[0] == '$') numberOfScatters++;
        symbol = reels[j][(indeces[j] + 1) % reelSize];
        if (symbol[0]  == '$') numberOfScatters++;
        symbol = reels[j][(indeces[j] + 2) % reelSize];
        if (symbol[0] == '$') numberOfScatters++;
    }
    return numberOfScatters;
}

/* Run
 *
 * Corre un juego, revisa cada linea a la que se aposto, y suma ganancias
 */
void Run(){
    losts += numberOfBettingLines;
    for (int i = 0; i < numberOfBettingLines; ++i){
        matches = 0;
        symbol = reels[0][(indeces[0] + (payLines[bettingLines[i]][0] - 1)) % reelSize];
        for (int j = 1; j < numberOfReels; ++j){
            nextSymbol = reels[j][(indeces[j] + (payLines[bettingLines[i]][j] - 1)) % reelSize];
            if ((symbol[0] == '*' || nextSymbol[0] == '*' || symbol[0] == nextSymbol[0]) && nextSymbol[0] != '$'){
                if (symbol[0] == '*') symbol = nextSymbol;
                matches++;
            }else{
                break;
            }
        }

        if(symbol[0] != '$') {
            gains += gainOfSymbols[symbol[0] - indexOfA][matches];
            if (gainOfSymbols[symbol[0] - indexOfA][matches] > 0){
                PrintPayline(payLines,(i));
                PrintWinningSymbol();
                PrintSymbolGains(gainOfSymbols[symbol[0] - indexOfA][matches]);
            }
        }
    }

    int scatterCount = GetNumberOfScatters();

    if(scatterCount > 0) {
        gains += scatterGain[scatterCount - 1];
        PrintScatters(scatterCount,scatterGain[scatterCount - 1]);
    }
    totalGains += gains;
}

void ReadSeedFromFile(string fileName){
    ifstream seedFile;
    string line;
    seedFile.open(fileName.c_str());
    if (seedFile.is_open()){
        getline(seedFile,line);
        seed=atoi(line.c_str());
       seedFile.close();
    }
    else cout << "Error al abrir el archivo de semilla";
}
void WriteSeedOnFile(string fileName){
    ofstream seedFile;
    seedFile.open(fileName.c_str());
    seedFile<<indeces[((sizeof(indeces)/sizeof(*indeces))-1)];
    seedFile.close();
}
/* Reset
 *
 * Reestablece algunas variables cada vez que se juega
 */
void Reset(){
    losts = 0;
    gains = 0;
    ReadSeedFromFile(seedFileName);
    srand(seed);
    for (int k = 0; k < numberOfReels; ++k) {
        indeces[k] = (indeces[k] + (rand() % reelSize)) %reelSize;
    }
    WriteSeedOnFile(seedFileName);
}

/* Play
 *
 * Un juego, corre las partidas indicadas por el usuario
 */
void Play(){
    for (int i = 1; i <= numberOfRunsPerGame; ++i){
        PrintPlayNumber(i);
        Reset();
        PrintIndexes(indeces);
        PrintSymbols();
        Run();
        WriteEnterToLogFile();
    }
    PrintTotalGains(totalGains);
}

/* PrintMachine
 *
 * Imprimir todos los datos de la maquina
 * Para pruebas
 */
void PrintMachine(){
    cout<<numberOfSymbols<<endl;
    for(int i=0;i<numberOfSymbols;i++){
        for(int j=0;j<5;j++){
            cout<<gainOfSymbols[i][j]<<" ";
        }
        cout<<endl;
    }
    for(int i=0;i<5;i++){
        cout<<wildGain[i]<<" ";
    }
    cout<<endl;

    for(int i=0;i<5;i++){
        cout<<scatterGain[i]<<" ";
    }
    cout<<endl;

    cout<<reelSize<<endl;

    for(int i=0;i<5;i++){
        for(int j=0;j<reelSize;j++){
            cout<<reels[i][j]<<" ";
        }
        cout<<endl;
    }

    cout<<numberOfPaylines<<endl;

    for(int i=0;i<numberOfPaylines;i++){
        for(int j=0;j<5;j++){
            cout<<payLines[i][j]<<" ";
        }
        cout<<endl;
    }

    cout<<numberOfRunsPerGame<<endl;
    cout<<numberOfBettingLines<<endl;

    for(int i=0;i<numberOfBettingLines;i++){
        cout<<bettingLines[i]<<" ";
    }
    cout<<endl;
}

void CleanAll(){

}

int main(int argc, char* argv[]){

    SetMachine(argv[1]);
    SetGame(argv[2]);
    seedFileName=argv[3];
    //PrintMachine();
    PrintTime();
    for (int i = 1; i <= numberOfGames; ++i){
        PrintGameTitle(i);
        Play();
    }

    CleanAll();
    return 0;
}


