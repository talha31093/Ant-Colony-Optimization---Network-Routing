#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cfloat>
#include <vector>
#include <cmath>
#include <fstream>

#define numNodes 11
#define numAnts 100
#define maxTime 100
#define infinity 9999

using namespace std;

int alpha = 3; // influence of pheromone in direction
int beta = 2; // influence of adjacent node distance
double rho = 0.01; // pheromone decrease factor
double Q = 2.0; // pheromone increase factor

void Graph(int dist[numNodes][numNodes])
{
srand(time(NULL));
for (int i = 0; i < numNodes; i++) {
for (int j = i; j < numNodes; j++) {
if (j == i) {
dist[i][j] = infinity;
} else {
int d = rand() % 10 + 1;
dist[i][j] = d;
dist[j][i] = d;
}
}
}
}

void printants(vector<int> ants[])
{
for (int i = 0; i < numAnts; i++) {
for (int j = 0; j < ants[i].size(); j++) {
cout << ants[i][j] << " ";
}
cout << endl;
}
cout << endl;
}

/*
void printdist(int dist[][numNodes])
{
for (int i = 0; i < numNodes; i++) {
for (int j = 0; j < numNodes; j++) {
cout << dist[i][j] << " ";
}
cout << endl;
}
cout << endl;
}
*/
void printpheromones(double pheromones[][numNodes])
{
for (int i = 0; i < numNodes; i++) {
for (int j = 0; j < numNodes; j++) {
cout << pheromones[i][j] << " ";
}
cout << endl;
}
cout << endl;
}

void InitPheromones(double pheromones[][numNodes])
{
for (int i = 0; i < numNodes; i++) {
for (int j = 0; j < numNodes; j++) {
pheromones[i][j] = 0.01;
}
}
}
/*
int IndexOfTarget(int target, vector<int> trail)
{
int i;
for (i = 0; i < trail.size(); i++) {
if (trail[i] == target) return i;
}
cout << "Target not Found in IndexOfTarget: " << i << endl;
}
*/
void InitAnts(vector<int> ants[numAnts])
{
for (int i = 0; i < numAnts; i++) {
int start = 0;
int vis[numNodes] = {0};
//int trail[numNodes];
vector<int> trail;
trail.push_back(start);
vis[start] = 1;

for (int j = 0; j < numNodes; j++) {
//trail[j] = j;
int r = rand() % numNodes;
r++;

if (vis[r] == 0) {
vis[r] = 1;
trail.push_back(r);
} else {
j--;
}

if (r == 10) {
break;
}
}
//random_shuffle(trail, trail + numNodes);
//int index = IndexOfTarget(start, trail);
int index = 0;
//swap(trail[0], trail[index]);
for (int j = 0; j < trail.size(); j++) {
ants[i].push_back(trail[j]);
}
}
}

double Length(vector<int> x, int dist[][numNodes])
{
double ans = 0.0;
for (int i = 0; i < x.size() - 1; i++) {
ans += dist[x[i]][x[i + 1]];
}
return ans;
}

void ShowLength(vector<int> ants[], int dist[][numNodes])
{
for (int i = 0; i < numAnts; i++) {
cout << i << ": [ ";
for (int j = 0; j < ants[i].size(); j++) {
cout << ants[i][j] << " ";
}
/*cout << " . . . ";
for (int j = numNodes - 4; j < numNodes; j++) {
cout << ants[i][j] << " ";
}*/
cout << "] len = ";
double len = Length(ants[i], dist);
cout << len << endl;
}
}

void BestTrail(vector<int> ants[], int dist[][numNodes], vector<int> &bestTrail)
{
double bestlength = Length(ants[0], dist);
int index = 0;
for (int i = 1; i < numAnts; i++) {
double len = Length(ants[i], dist);
if (len < bestlength) {
bestlength = len;
index = i;
}
}

for (int i = 0; i < ants[index].size(); i++) {
bestTrail.push_back(ants[index][i]);
}
}

void Probability(int k, int nodeX, bool visited[], double pheromones[][numNodes], int dist[][numNodes], double probs[])
{
double tau[numNodes];
double sum = 0.0;
for (int i = 0; i < numNodes; i++) {
if (i == nodeX) {
tau[i] = 0.0;
} else if (visited[i] == true) {
tau[i] = 0.0;
} else {
tau[i] = (pow(pheromones[nodeX][i], alpha*1.0)) * (pow((1.0 / dist[nodeX][i]*1.0), beta*1.0));
if (tau[i] < 0.0001) {
tau[i] = 0.0001;
} else if (tau[i] > DBL_MAX / numNodes * 100) {
tau[i] = DBL_MAX / numNodes * 100;
}
}
sum += tau[i];
}
for (int i = 0; i < numNodes; i++) {
probs[i] = tau[i] / sum;
}
}

int NextNode(int k, int nodeX, bool visited[], double pheromones[][numNodes], int dist[][numNodes])
{
double probs[numNodes];
Probability(k, nodeX, visited, pheromones, dist, probs);
double cum[numNodes + 1];
cum[0] = 0.0;
for (int i = 0; i < numNodes; i++) {
cum[i + 1] = cum[i] + probs[i];
}
double p = (double)rand() / (double)RAND_MAX;
for (int i = 0; i < numNodes; i++) {
if (p >= cum[i] && p < cum[i + 1]) {
return i;
}
}
cout << "Failure\n";
}


void BuildTrail(int k, int start, double pheromones[][numNodes], int dist[][numNodes], vector<int> &newTrail)
{
// int trail[numNodes];
bool visited[numNodes];
vector<int> trail;

for (int i = 0; i < numNodes; i++) {
visited[i] = false;
}
trail.push_back(start);
visited[start] = true;
for (int i = 0; i < numNodes - 1; i++) {
int nodeX = trail[i];
int next = NextNode(k, nodeX, visited, pheromones, dist);
trail.push_back(next);
visited[next] = true;

if (next == 10) {
break;
}
}
for (int i = 0; i < trail.size(); i++) {
newTrail.push_back(trail[i]);
}
}

void UpdateAnts(vector<int> ants[], double pheromones[][numNodes], int dist[][numNodes])
{
for (int i = 0; i < numAnts; i++) {
ants[i].clear();
int start = 0;
vector<int> newTrail;
BuildTrail(i, start, pheromones, dist, newTrail);
for (int j = 0; j < newTrail.size(); j++) {
ants[i].push_back(newTrail[j]);
}
}
}

bool EdgePresentinTrail(int nodeX, int nodeY, vector<int> trail)
{
/* int lastIndex = trail.size() - 1;
int index = IndexOfTarget(nodeX, trail);

if (index == 0 && trail[1] == nodeY) return true;
else if (index == 0 && trail[lastIndex] == nodeY) return true;
else if (index == 0) return false;
else if (index == lastIndex && trail[lastIndex - 1] == nodeY) return true;
else if (index == lastIndex && trail[0] == nodeY) return true;
else if (index == lastIndex) return false;
else if (trail[index - 1] == nodeY) return true;
else if (trail[index + 1] == nodeY) return true;
else return false;
*/

for (int i = 0; i < trail.size() - 1; i++) {
if (trail[i] == nodeX && trail[i+1] == nodeY) {
return true;
} else if (trail[i] == nodeY && trail[i+1] == nodeX) {
return true;
}
}
return false;
}

void UpdatePheromones(vector<int> ants[], double pheromones[][numNodes], int dist[][numNodes])
{
for (int i = 0; i < numNodes; i++) {
for (int j = i + 1; j < numNodes; j++) {
for (int k = 0; k < numAnts; k++) {
double length = Length(ants[k], dist);
double decrease = (1.0 - rho) * pheromones[i][j];
double increase = 0.0;
if (EdgePresentinTrail(i, j, ants[k]) == true) {
increase = Q / length;
}
pheromones[i][j] = increase + decrease;
if (pheromones[i][j] < 0.0001) {
pheromones[i][j] = 0.0001;
} else if (pheromones[i][j] > 100000.0) {
pheromones[i][j] = 100000.0;
}
pheromones[j][i] = pheromones[i][j];
}
}
}
}

void Display(vector<int> besttrail)
{
for (int i = 0; i < besttrail.size(); i++) {
cout << besttrail[i] << " ";
/*if (i > 0 && i % 20 == 0) {
cout << endl;
}*/
}
cout << endl;
}

int main()
{
cout << "\nBegin Ant Colony Demo:\n\n";
cout << "Number of nodes in the problem: " << numNodes << endl;
cout << "Number of ants: " << numAnts << endl;
cout << "Maximum Time: " << maxTime << endl;
cout << "Alpha (pheromone influence): " << alpha << endl;
cout << "Beta (local node influence): " << beta << endl;
cout << "Rho (pheromone evaporation coefficient): " << rho << endl;
cout << "Q (pheromone deposit factor) " << Q << endl;
//cout << "\nCreating Dummy Undirected Graph with Random edge lengths:\n";

int dist[numNodes][numNodes];
Graph(dist); // Making Graph
ofstream f;
f.open("Output.txt");
if (f.good()) {
//cout << "hello";
}
f << numNodes;
f << endl;
for (int i = 0; i < numNodes; i++) {
for (int j = 0; j < numNodes; j++) {
f << dist[i][j];
f << " ";
}
f << endl;
}
// printdist(dist);

cout << "\nInitialising ants to random trails:\n";	// Initialising trails to ants
// int ants[numAnts][numNodes];
vector<int> ants_1[numAnts];
InitAnts(ants_1);
// printants(ants);
ShowLength(ants_1, dist);

vector<int> bestTrail;
BestTrail(ants_1, dist, bestTrail);

double bestlength = Length(bestTrail, dist);
cout << "\nBest Initial Trail Length: " << bestlength << endl << endl;

cout << "\nInitialising Pheromones on trails: \n";
double pheromones[numNodes][numNodes]; //Initialising Pheromones
InitPheromones(pheromones);
int time = 0;
while (time < maxTime) {

UpdateAnts(ants_1, pheromones, dist);
UpdatePheromones(ants_1, pheromones, dist);

vector<int> currBestTrail;

BestTrail(ants_1, dist, currBestTrail);

double currbestlength = Length(currBestTrail, dist);

// The code given below is to implement link failure. Uncomment the commented part to verify.
/*if (time == 50) {
cout << "Till now The best trail is\n ";
Display(bestTrail);
dist[bestTrail[0]][bestTrail[1]] = infinity;
bestlength = infinity;
cout << "Link Failure in " << bestTrail[0] << " " << bestTrail[1] << endl;
}*/

if (currbestlength < bestlength) {
bestTrail.clear();
bestlength = currbestlength;
for (int i = 0; i < currBestTrail.size(); i++) {
bestTrail.push_back(currBestTrail[i]);
}
cout << "New Best Trail of: " << bestlength << " found at time : " << time << endl;
}
++time;
}

cout << "\nTime Complete:\n";
cout << "\nBest Trail Found:\n";

Display(bestTrail);

cout << "\nLength of Best trail found: " << bestlength << endl;
cout << "\nEnd ACO Demo\n" << endl;

return 0;
}
