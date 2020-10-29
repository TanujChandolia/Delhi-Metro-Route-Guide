#include<iostream>
#include<bits/stdc++.h>
#include<fstream>
#include<Windows.h>

using namespace std;

class StationInfo{
public:
   int number;
   int distance;
   string line;

   StationInfo(int a,int b,string c){
      number = a;
      distance = b;
      line = c;
   }
};

struct Compare{
   bool operator()(StationInfo const& s1, StationInfo const& s2){
      return s1.distance < s2.distance;
   }
};

int totalStations = 286;
unordered_map<string,int> numberRef; // for station to number
map<int,string> stationRef;  // for number to station
vector<vector<string>> lines(totalStations);
int stationCount = 0;

string uppercase(string x){
   transform(x.begin(),x.end(),x.begin(),::toupper);
   return x;
}

string lowercase(string x){
   transform(x.begin(),x.end(),x.begin(),::tolower);
   return x;
}

string trim(string x){
   int i = 0;
   while(x[i] == ' '){
      i++;
   }
   x = x.substr(i);
   i = x.length() - 1;
   while(x[i] == ' '){
      i--;
   }
   x = x.substr(0,i+1);
   return x;
}

void fillMaps(string s1, string s2){
     if(numberRef.find(s1) == numberRef.end()){
          numberRef[s1] = stationCount;
          stationRef[stationCount] = s1;
          stationCount++;
     }
     if(numberRef.find(s2) == numberRef.end()){
          numberRef[s2] = stationCount;
          stationRef[stationCount] = s2;
          stationCount++;
     }
}

void fillLines(int n1, int n2, string currLine){
   bool found = false;
   for(string i:lines[n1]){
      if(i == currLine){
         found =  true;
         break;
      }
   }

   if(!found) lines[n1].push_back(currLine);
   else found = false;

   for(string i:lines[n2]){
      if(i == currLine){
         found =  true;
         break;
      }
   }

   if(!found) lines[n2].push_back(currLine);
}

void readFile(vector<vector<vector<int>>> & stationGraph){
     string line,currLine;
     ifstream fin;
     fin.open("metro.txt");

     while(!fin.eof()){
          getline(fin,line);
          string station1,station2,dist;
          bool comma = false, comma_again = false;
          if(line.empty()) continue;
          if(line[0] == '/'){
             currLine = "";
             int i = 6;
             while(line[i] != ' '){
                currLine += line[i];
                i++;
             }
             currLine = lowercase(currLine);
             continue;
          }
          for(char c:line){
               if(c == '[' || c == ']') continue;
               if(c == ','){
                 if(comma) comma_again = true;
                 comma = true;
                 continue;
               }
               if(comma_again){
                  dist += c;
                  continue;
               }
               if(!comma) station1 += c;
               else station2 += c;
          }

          if(!station1.empty() && !station2.empty()){
               fillMaps(station1,station2);
          }
          if(!dist.empty()){
               int d = stoi(dist);
               int n1 = numberRef[station1];
               int n2 = numberRef[station2];
               fillLines(n1,n2,currLine);
               vector<int> v = {n2,d};
               vector<int> w = {n1,d};
               stationGraph[n1].push_back(v);
               stationGraph[n2].push_back(w);
          }
     }
     fin.close();
}

void printPath(vector<int> &parent, int e){
     vector<string> path;
     int x = e;
     while(x >= 0){
          // cout<<stationRef[x]<<" ";
          path.push_back(stationRef[x]);
          x = parent[x];
     }
     cout<<path.size()<<" stations.\n";

     reverse(path.begin(),path.end());
     vector<string> lineColor(path.size());

     if(lines[numberRef[path[0]]].size() == 1){
        lineColor[0] = lines[numberRef[path[0]]][0];
     }
     else{
        for(string s:lines[numberRef[path[0]]]){
           for(string p:lines[numberRef[path[1]]]){
             if(p == s){
                 lineColor[0] = p;
                 break;
             }
           }
        }
     }

    for(int i = 1; i < path.size(); i++){
        bool found = false;
        for(string s:lines[numberRef[path[i]]]){
           if(s == lineColor[i-1]){
              lineColor[i] = s;
              found = true;
              break;
           }
        }

        if(!found){
           for(string s:lines[numberRef[path[i-1]]]){
              for(string p:lines[numberRef[path[i]]]){
                 if(p == s){
                    lineColor[i] = p;
                    break;
                 }
              }
           }
        }
    }

     cout<<"Route to be taken: \n\n";
     cout<<"   STATIONS";
     cout.width(32);
     cout<<"LINE";
     cout.width(38);
     cout<<"INTERCHANGES\n";
     for(int i = 0; i < 90; i++) cout<<"-";
     cout<<"\n";

     for(int i = 0; i < path.size(); i++){
        Sleep(100);
        string currS = uppercase(path[i]);
        string currL = uppercase(lineColor[i]);

        cout<<"|o "<<currS;
        cout.width(35-path[i].length());
        cout<<right<<"|    --> ("<<currL<<" LINE)";
        if(i != path.size()-1){
           if(lineColor[i+1] != lineColor[i]){
              string ch = uppercase(lineColor[i+1]);
              cout.width(32-lineColor[i].length());
              cout<<right<< "||INTERCHANGE TO "<<ch<<" LINE||";
           }
        }
        cout<<"\n";

     }
     for(int i = 0; i < 90; i++) cout<<"-";
     cout<<"\n";
}

int correctSearches(string s1, string s2){
   int m = s1.size();
   int n = s2.size();
   vector<vector<int>> dp(m+1,vector<int>(n+1,0));
   for(int i = 0; i <= n; i++){
      dp[0][i] = i;
   }
   for(int i = 0; i <= m; i++){
      dp[i][0] = i;
   }
   for(int i = 1; i <= m; i++){
      for(int j = 1; j <= n; j++){
         if(s1[i-1] == s2[j-1])
            dp[i][j] = dp[i-1][j-1];
         else{
            dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
         }
      }
   }

   return dp[m][n];
}

string fixStation(string station){
   //editDistance to be used here
   int minError = INT_MAX;
   unordered_set<string> possibleStations;
   vector<string> containsSubstring;

   for(auto st:numberRef){
      string corrStation = st.first;
      if(corrStation.find(station) != string::npos)
         containsSubstring.push_back(corrStation);
      int error = correctSearches(station,corrStation);
      if(error < minError){
         minError = error;
         possibleStations.clear();
         possibleStations.insert(corrStation);
      }
      else if(error == minError){
         possibleStations.insert(corrStation);
      }
   }

   for(string s:containsSubstring){
      possibleStations.insert(s);
   }
   vector<string> possible(possibleStations.begin(),possibleStations.end());
   cout<<"Possible Correct Stations: \n";
   int count = 1;
   for(string s:possible){
      Sleep(50);
      cout<<count<<") "<<uppercase(s)<<"\n";
      count++;
   }


   int num;
   cout<<"Enter the number to choose a station: ";
   cin >> num;
   while(num > possible.size()){
      cout<<"\n***Invalid Number.*** Please enter again: ";
      cin >> num;
   }
   return possible[num-1];
}

void djikstra(vector<vector<vector<int>>> &stationGraph, string start, string end){
     start = lowercase(start);
     end = lowercase(end);
     int s,e;
     if(numberRef.find(start) == numberRef.end()){
          cout<<"\n***Invalid Starting Station.***\n\n";
          start = fixStation(start);
     }
     s = numberRef[start];


     if(numberRef.find(end) == numberRef.end()){
          cout<<"\n***Invalid Ending Station.***\n\n";
          end = fixStation(end);
     }
     e = numberRef[end];
     int interchangeCost = 5;

     priority_queue<StationInfo, vector<StationInfo>, Compare> pq;
     vector<int> parent(totalStations,-1);
     vector<int> dist(totalStations,1000);

     dist[s] = 0;
     for(int i = 0; i < lines[s].size(); i++){
        pq.push(StationInfo(s,0,lines[s][i]));
     }

     //cout<<s<<" "<<e<<"\n";
     while(!pq.empty()){
          StationInfo si = pq.top();
          int u = si.number;
          string line = si.line;
          //cout<<"Current Station: "<<stationRef[u]<<"\n";

          pq.pop();

          vector<vector<int>> allReachableStations = stationGraph[u];
          for(vector<int> station: allReachableStations){
               int nextStation = station[0];
               int distToReach = station[1];
               int interchange = 0;
               string lineforthis = "";


               for(string currSL: lines[u]){
                  for(string newSL: lines[nextStation]){
                     if(currSL == newSL){
                        lineforthis = currSL;
                     }
                  }
               }

               if(lineforthis != line) interchange = interchangeCost;

               if(dist[nextStation] >= dist[u] + distToReach + interchange){
                    dist[nextStation] = dist[u] + distToReach + interchange;
                    parent[nextStation] = u;
                    pq.push(StationInfo(nextStation,dist[nextStation],lineforthis));
               }
          }



     }

     // for(int i = 0; i < totalStations; i++){
     //   if(dist[i] != 1000)
     //      cout<<i<<": "<<stationRef[i]<<" "<<dist[i]<<"\n";
     // }
     // cout<<"\n";
     Sleep(100);
     cout<<"\nSmallest Route to reach "<<uppercase(end)<<" from "<<uppercase(start)<<" is ";
     printPath(parent,e);
}

void printStations(){
     cout<<"Current Stations:\n\n";
     for(auto i:stationRef){
          string x = i.second;
          x = uppercase(x);
          cout<<i.first<<": "<<x<<"\n";
     }
     cout<<"\n\n\n";
}

void printGraph(vector<vector<vector<int>>> &stationGraph){
     cout<<"Current Metro Routes:\n\n";
     for(int i = 0 ; i < totalStations; i++){
          vector<vector<int>> v = stationGraph[i];
          if(v.empty()) continue;
          cout<<i<<": ";
          for(vector<int> x:v){
               cout<<"{"<<x[0]<<","<<x[1]<<"}"<<" ";
          }
          cout<<"\n";
     }
     cout<<"\n\n";
}

void printLines(){
   for(int i = 0; i < totalStations; i++){
      if(stationRef.find(i) == stationRef.end()) continue;
      cout<<stationRef[i]<<": ";
      for(int j = 0; j < lines[i].size(); j++ ){
         cout<<lines[i][j]<<" ";
      }
      cout<<"\n";
   }
}

void printWelcome(){
   for(int i = 0; i < 90; i++) cout<<"=";
   cout<<"\n\n\n";
   cout.width(63);
   cout<<right<<"WELCOME TO DELHI METRO ROUTE GUIDE";
   cout<<"\n\n\n";
   for(int i = 0; i < 90; i++) cout<<"=";
   cout<<"\n\n";
   cout.width(66);
   cout<<right<<"Where would you like to travel today?\n\n";
}

int main(){
     vector<vector<vector<int>>> stationGraph(totalStations);
     readFile(stationGraph);

     // printStations();
     // printGraph(stationGraph);
     // printLines();

     printWelcome();
     string start,end;
     cout<<"Enter Starting Station: ";
     getline(cin,start);
     cout<<"Enter Ending Station: ";
     getline(cin,end);

     start = trim(start);
     end = trim(end);

     if(start == end){
        cout<<"\n"<<"* Starting and Ending Stations cannot be same *";
        return 0;
     }
     djikstra(stationGraph,start,end);

     return 0;
}
