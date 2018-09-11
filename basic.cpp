#include <bits/stdc++.h>
using namespace std;
struct point
{
	double x;
	double y;
	static bool comp_x( const point *c1, const point *c2 ) { return c1->x < c2->x; }
	static bool comp_y( const point *c1, const point *c2 ) { return c1->y < c2->y; }
};

/*Node used in Quad Tree*/
struct qdnode
{
	bool isleaf; //is current qdnode is leaf or internal
	string direction;
	int users;
	int lazy;
	map<int,point*> stored;
	struct region *reg;
	struct qdnode *NW, *NE, *SE, *SW, *parent;
};

struct region
{
	double xmax, xmin, ymax, ymin ;
};
bool comp(double a,double b){
	if (abs(a-b)<0.000000001)
		return true;
	else 
		return false;
}
/*Quad Tree Class*/
class qtree
{
    struct region *larRect; //stores the largest rectangle enclosing all the points in the dataset
	struct qdnode *root;
    public:
        qtree();
	    int getHeight();
	    int _getHeight(qdnode *current);
        /*void print();
        void _print(qdnode *current);*/
        
	    region* query(qdnode *current, int &expanded,int k,double area);

        bool TestIntersect(region *r1, region *r2);
        bool PointInside(point *p, region *reg);
        bool Inside(region *r1, region *r2);
        void remove(qdnode *node,int &expanded);
        void update(qdnode *current,point *pnt,int &expanded,int userId,map<int,qdnode*> &hash);
        void updateUser(point *newL,int &expanded,int userId,map<int,qdnode*> &hash,map<int,point*> &location);
        /*void Visualize();
        void _Visualize(qdnode *myroot);*/
        qdnode *makeQTree(std::vector<point*> &data,std::vector<pair<point*,int> > &dataUser,double d,map<int,qdnode*> &mp);

	    /*Recursive utility function to create the KDTree*/
	    qdnode *_makeQTree(std::vector<pair<point*,int> > &dataUser,string direction, region *current_rect, qdnode *parent,double d,map<int,qdnode*> &mp);
};

/*void qtree::Visualize(){
	_Visualize(this->root);
}
void qtree::_Visualize(qdnode *myroot)
{
	if (root == NULL)
	{
		return;
	}
	cout<<"\n ---------------Visualization of Quad Tree by printing Level order Nodes--------------\n"<<endl;
	deque<qdnode*> deque1 ;
	deque<qdnode*> deque2 ;
	deque1.push_back(myroot);
	int level = 0;
	std::cout << "Printing Nodes of Quad Tree at Level " << level << " :" << std::endl;
	level++;
	while (deque1.size() > 0 || deque2.size() > 0)
	{
		if (deque1.size() == 0)
		{
			while (deque2.size() != 0)
			{
				deque1.push_back(deque2.front());
				deque2.pop_front();
			}
			if (deque2.size() == 0)
			{
				std::cout << std::endl;
				std::cout << "Printing Nodes of Quad Tree at Level " << level << " :" << std::endl;
				level++;
			}
		}
		
		qdnode *curr = deque1.front();
		deque1.pop_front();
		std::cout << "Is Leaf :" << curr->isleaf;
		if (curr->parent != NULL)
		{
			cout<<" ,Current Node is "<<curr->direction<<" child of Its Parent";
		}
		else
		{
			std::cout << " ,No Parent";
		}
		if (curr->isleaf == false)
		{
			deque2.push_back(curr->NW);
			deque2.push_back(curr->NE);
			deque2.push_back(curr->SE);
			deque2.push_back(curr->SW);
			std::cout << " ,Split point : x = " << curr->split->x << " , y = " << curr->split->y;
		}
		else 
		{
			if (curr->stored !=NULL)
				cout<<" ,Stored Point: x = "<<curr->stored->x<<" , y = "<<curr->stored->y;
			else
				cout<<" ,No Point Stored";
		}
		std::cout << std::endl;
	}
}
void qtree::print(){
    _print(this->root);
}
void qtree::_print(qdnode *nod)
{
	cout << "isleaf " << nod->isleaf << " direction " << nod->direction;
	if (nod->split != NULL)
	{
		cout << " split x = " << nod->split->x << " y= " << nod->split->y;
	}
	if (nod->stored != NULL)
	{
	    cout << " store x = " << nod->stored->x << " y= " << nod->stored->y;
	}
	cout <<endl;
	if (nod->NW != NULL)
	{
		_print(nod->NW);
	}
	if (nod->NE != NULL)
	{
		_print(nod->NE);
	}
	if (nod->SE != NULL)
	{
		_print(nod->SE);
	}
	if (nod->SW != NULL)
	{
		_print(nod->SW);
	}
}
*/
double getArea(region *rect){
    return ((rect->xmax-rect->xmin)*(rect->ymax-rect->ymin));
}
/*constructor for quad tree*/
qtree::qtree(){
    this->root =NULL;
    this->larRect =NULL;
}
qdnode *qtree::makeQTree(std::vector<point*> &data,std::vector<pair<point*,int> > &dataUser,double d,map<int,qdnode*> &mp)
{
    if (data.size()==0){
        return NULL;
    }
	std::vector<point*> data_x;
	data_x.insert(data_x.end(), data.begin(), data.end());
	std::vector<point*> data_y;
	data_y.insert(data_y.end(), data.begin(), data.end());
	std::sort(data_x.begin(), data_x.end(), point::comp_x); //sort the dataset on the basis of x coordinates of points
	std::sort(data_y.begin(), data_y.end(), point::comp_y); //Sort the dataset on the basis of y coordinates of points
    this->larRect =new region;
    this->root = new qdnode; 
	double x_ran = ceil(data_x[data_x.size()-1]->x)-floor(data_x[0]->x);
	double y_ran = ceil(data_y[data_y.size()-1]->y)-floor(data_y[0]->y);
	double ran = max(x_ran,y_ran);
	double sq = d;
	while (sq<ran){
	    sq = 2*sq;
	}
	larRect->xmax = floor(data_x[0]->x)+sq;
	larRect->xmin = floor(data_x[0]->x);
	larRect->ymax = floor(data_y[0]->y)+sq;
	larRect->ymin = floor(data_y[0]->y);
	//cout<<sq<<" "<<larRect->xmax<<" "<<larRect->xmin<<" "<<larRect->ymax<<" "<<larRect->ymin<<"  "<<(d*d)<<endl;
	root->parent = NULL; // parent will be null for root

	if (data.empty())
	{
		this->root->isleaf = true;
		this->root->direction = "";
		this->root->users = 0;
		this->root->lazy = 0;
		this->root->reg = larRect;
		this->root->NW = NULL;
		this->root->NE = NULL;
		this->root->SW = NULL;
		this->root->SE = NULL;
		this->root->parent = NULL;
		return this->root;
	}
	else if (getArea(this->larRect)<=d*d+(0.001)*(d*d)){
	    this->root->isleaf = true;
		this->root->direction = "";
		this->root->users = (int)dataUser.size();
		this->root->lazy = 0;
		this->root->reg = larRect;
		this->root->NW = NULL;
		this->root->NE = NULL;
		this->root->SW = NULL;
		this->root->SE = NULL;
		this->root->parent = NULL;
		for (int i=0;i<(int)dataUser.size();i++){
		    this->root->stored[dataUser[i].second] = dataUser[i].first;
		    mp[dataUser[i].second] = this->root;
		}
		return this->root;   
	}
	else
	{
		this->root->isleaf = false;
		this->root->direction = "";
		this->root->users = (int)dataUser.size();
		this->root->lazy = 0;
		this->root->reg = larRect;
		this->root->parent = NULL;
		
		double mid_x = larRect->xmin + ((larRect->xmax*1.0 - larRect->xmin*1.0) / (2*1.0));
		double mid_y = larRect->ymin + ((larRect->ymax*1.0 - larRect->ymin*1.0) / (2*1.0));

		/*Vectors to contains the data points in the four regions*/
		std::vector<pair<point*,int> > data_NW;
		std::vector<pair<point*,int> > data_NE;
		std::vector<pair<point*,int> > data_SE;
		std::vector<pair<point*,int> > data_SW;

		/*Creating the four regions*/
		region *NW_rect; 
		region *NE_rect; 
		region *SE_rect; 
		region *SW_rect; 
        NW_rect = new region;
        NE_rect = new region;
        SE_rect = new region;
        SW_rect = new region;
        NW_rect->xmax = mid_x;NW_rect->xmin = larRect->xmin;NW_rect->ymax = larRect->ymax;NW_rect->ymin = mid_y;
        NE_rect->xmax = larRect->xmax;NE_rect->xmin = mid_x;NE_rect->ymax = larRect->ymax;NE_rect->ymin = mid_y;
        SE_rect->xmax = larRect->xmax;SE_rect->xmin = mid_x;SE_rect->ymax = mid_y;SE_rect->ymin = larRect->ymin;
        SW_rect->xmax = mid_x;SW_rect->xmin = larRect->xmin;SW_rect->ymax = mid_y;SW_rect->ymin = larRect->ymin;
        
        /*Putting the datapoints in their corresponding regions*/
		for (int i = 0;i < dataUser.size();i++)
		{
			if (PointInside(dataUser[i].first,NW_rect) == true)
			{
				data_NW.push_back(dataUser[i]);
			}
			else if (PointInside(dataUser[i].first,NE_rect) == true)
			{
				data_NE.push_back(dataUser[i]);
			}
			else if (PointInside(dataUser[i].first,SE_rect) == true)
			{
				data_SE.push_back(dataUser[i]);
			}
			else
			{
				data_SW.push_back(dataUser[i]);
			}
		}

		/*Calling the children*/
        string nw="NW";string ne="NE";string sw="SW";string se="SE"; 
		this->root->NW = _makeQTree(data_NW,nw,NW_rect,this->root,d,mp);
		this->root->NE = _makeQTree(data_NE,ne,NE_rect,this->root,d,mp);
		this->root->SE = _makeQTree(data_SE,se,SE_rect,this->root,d,mp);
		this->root->SW = _makeQTree(data_SW,sw,SW_rect,this->root,d,mp);
		
	}

	return this->root;
}

qdnode *qtree::_makeQTree(std::vector<pair<point*,int> > &dataUser,string direction, region *current_rect, qdnode *parent,double d,map<int,qdnode*> &mp)
{
	qdnode *current = new qdnode;
	current->parent = parent; // set the parent of current qdnode
	current->direction = direction; // set whether current qdnode is left or right child of parent
	/*cout<<direction<<" "<<current_rect->xmax<<" "<<current_rect->xmin<<" "<<current_rect->ymax<<" "<<current_rect->ymin<<" "<<getArea(current_rect)<<"   "<<(d*d)<<"   "<<dataUser.size()<<endl;
	for (int i=0;i<dataUser.size();i++){
		cout<<"( "<<dataUser[i].first->x<<" , "<<dataUser[i].first->y<<" )  ,  ";
	}
	cout<<endl;*/
	if (getArea(current_rect)<=d*d+(0.001)*(d*d)){
	    current->isleaf = true;
		current->users = (int)dataUser.size();
		current->lazy = 0;
		current->reg = current_rect;
		current->NW = NULL;
		current->NE = NULL;
		current->SW = NULL;
		current->SE = NULL;
		for (int i=0;i<(int)dataUser.size();i++){
		    current->stored[dataUser[i].second] = dataUser[i].first;
		    mp[dataUser[i].second] = current;
		}
		return current;   
	}
	else
	{
		current->isleaf = false;
		current->users = (int)dataUser.size();
		current->lazy = 0;
		current->reg = current_rect;
		
		double mid_x = current_rect->xmin + ((current_rect->xmax*1.0 - current_rect->xmin*1.0) / (2*1.0));
		double mid_y = current_rect->ymin + ((current_rect->ymax*1.0 - current_rect->ymin*1.0) / (2*1.0));

		/*Vectors to contains the data points in the four regions*/
		std::vector<pair<point*,int> > data_NW;
		std::vector<pair<point*,int> > data_NE;
		std::vector<pair<point*,int> > data_SE;
		std::vector<pair<point*,int> > data_SW;

		/*Creating the four regions*/
		region *NW_rect; 
		region *NE_rect; 
		region *SE_rect; 
		region *SW_rect; 
        NW_rect = new region;
        NE_rect = new region;
        SE_rect = new region;
        SW_rect = new region;
        NW_rect->xmax = mid_x;NW_rect->xmin = current_rect->xmin;NW_rect->ymax = current_rect->ymax;NW_rect->ymin = mid_y;
        NE_rect->xmax = current_rect->xmax;NE_rect->xmin = mid_x;NE_rect->ymax = current_rect->ymax;NE_rect->ymin = mid_y;
        SE_rect->xmax = current_rect->xmax;SE_rect->xmin = mid_x;SE_rect->ymax = mid_y;SE_rect->ymin = current_rect->ymin;
        SW_rect->xmax = mid_x;SW_rect->xmin = current_rect->xmin;SW_rect->ymax = mid_y;SW_rect->ymin = current_rect->ymin;
        
        /*Putting the datapoints in their corresponding regions*/
		for (int i = 0;i < dataUser.size();i++)
		{
			if (PointInside(dataUser[i].first,NW_rect) == true)
			{
				data_NW.push_back(dataUser[i]);
			}
			else if (PointInside(dataUser[i].first,NE_rect) == true)
			{
				data_NE.push_back(dataUser[i]);
			}
			else if (PointInside(dataUser[i].first,SE_rect) == true)
			{
				data_SE.push_back(dataUser[i]);
			}
			else
			{
				data_SW.push_back(dataUser[i]);
			}
		}

		/*Calling the children*/
        string nw="NW";string ne="NE";string sw="SW";string se="SE"; 
		current->NW = _makeQTree(data_NW,nw,NW_rect,current,d,mp);
		current->NE = _makeQTree(data_NE,ne,NE_rect,current,d,mp);
		current->SE = _makeQTree(data_SE,se,SE_rect,current,d,mp);
		current->SW = _makeQTree(data_SW,sw,SW_rect,current,d,mp);
		
	}

	return current;
}


/*returns the maximum of four numbers a,b,c and d*/
int maxi(int a, int b, int c, int d)
{
	return max(max(max(a, b), c),d);
}

/*function to get the height of quadtree*/
int qtree::getHeight(){
    return _getHeight(this->root);
}
/*Utility function to get the height of quad tree*/
int qtree::_getHeight(qdnode *current)
{
	
	if (current->isleaf == true) //Assuming leaf qdnode is at height 1
	{
		return 1;
	}
	else
	{
		return (maxi(_getHeight(current->NW),_getHeight(current->NE),_getHeight(current->SE),_getHeight(current->SW)) + 1);
	}
}

/*Check whether region r1 intersects with refion r2 or not*/
bool qtree::TestIntersect(region *r1, region *r2)
{
	// If one rectangle is on left side of other
	if (r1->xmin > r2->xmax || r2->xmin > r1->xmax)
	{
		return false;
	}

	// If one rectangle is above other
	if (r1->ymax < r2->ymin || r2->ymax < r1->ymin)
	{
		return false;
	}

	return true;
}

/*Check whether point p is inside region reg or not*/
bool qtree::PointInside(point *p, region *reg)
{
	if (p->y >= reg->ymin && p->y <= reg->ymax && p->x >= reg->xmin && p->x <= reg->xmax)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*Check if region r1 is inside region r2 or not*/
bool qtree::Inside(region *r1, region *r2)
{
	if (r1->xmax <= r2->xmax && r1->xmin >= r2->xmin && r1->ymax <= r2->ymax && r1->ymin >= r2->ymin)
	{
		return true;
	}
	else
	{
		return false;
	}
}



region* qtree::query(qdnode *current, int &expanded,int k,double area){
	expanded++;
	if (current->users >= k && getArea(current->reg)>=area){
		return current->reg;
	}
	else if (current->parent!=NULL){
		expanded = expanded + 2;
		region *regH;
		regH = new region;
		region *regV;
		regV = new region;
		int userH = 0;
		int userV = 0;
		if (current->direction=="NW"){
			userH = current->users + current->parent->NE->users;
			regH->ymax = max(current->reg->ymax,current->parent->NE->reg->ymax);
			regH->ymin = min(current->reg->ymin,current->parent->NE->reg->ymin);
			regH->xmax = max(current->reg->xmax,current->parent->NE->reg->xmax);
			regH->xmin = min(current->reg->xmin,current->parent->NE->reg->xmin);

			userV = current->users + current->parent->SW->users;
			regV->ymax = max(current->reg->ymax,current->parent->SW->reg->ymax);
			regV->ymin = min(current->reg->ymin,current->parent->SW->reg->ymin);
			regV->xmax = max(current->reg->xmax,current->parent->SW->reg->xmax);
			regV->xmin = min(current->reg->xmin,current->parent->SW->reg->xmin);
		}
		else if (current->direction=="NE"){
			userH = current->users + current->parent->NW->users;
			regH->ymax = max(current->reg->ymax,current->parent->NW->reg->ymax);
			regH->ymin = min(current->reg->ymin,current->parent->NW->reg->ymin);
			regH->xmax = max(current->reg->xmax,current->parent->NW->reg->xmax);
			regH->xmin = min(current->reg->xmin,current->parent->NW->reg->xmin);

			userV = current->users + current->parent->SE->users;
			regV->ymax = max(current->reg->ymax,current->parent->SE->reg->ymax);
			regV->ymin = min(current->reg->ymin,current->parent->SE->reg->ymin);
			regV->xmax = max(current->reg->xmax,current->parent->SE->reg->xmax);
			regV->xmin = min(current->reg->xmin,current->parent->SE->reg->xmin);
		}		
		else if (current->direction=="SE"){
			userH = current->users + current->parent->SW->users;
			regH->ymax = max(current->reg->ymax,current->parent->SW->reg->ymax);
			regH->ymin = min(current->reg->ymin,current->parent->SW->reg->ymin);
			regH->xmax = max(current->reg->xmax,current->parent->SW->reg->xmax);
			regH->xmin = min(current->reg->xmin,current->parent->SW->reg->xmin);

			userV = current->users + current->parent->NE->users;
			regV->ymax = max(current->reg->ymax,current->parent->NE->reg->ymax);
			regV->ymin = min(current->reg->ymin,current->parent->NE->reg->ymin);
			regV->xmax = max(current->reg->xmax,current->parent->NE->reg->xmax);
			regV->xmin = min(current->reg->xmin,current->parent->NE->reg->xmin);
		}
		else{
			userH = current->users + current->parent->SE->users;
			regH->ymax = max(current->reg->ymax,current->parent->SE->reg->ymax);
			regH->ymin = min(current->reg->ymin,current->parent->SE->reg->ymin);
			regH->xmax = max(current->reg->xmax,current->parent->SE->reg->xmax);
			regH->xmin = min(current->reg->xmin,current->parent->SE->reg->xmin);

			userV = current->users + current->parent->NW->users;
			regV->ymax = max(current->reg->ymax,current->parent->NW->reg->ymax);
			regV->ymin = min(current->reg->ymin,current->parent->NW->reg->ymin);
			regV->xmax = max(current->reg->xmax,current->parent->NW->reg->xmax);
			regV->xmin = min(current->reg->xmin,current->parent->NW->reg->xmin);
		}
		/*cout<<current->direction<<"  START "<<current->reg->xmax<<" "<<current->reg->xmin<<" "<<current->reg->ymax<<" "<<current->reg->ymin<<endl;
		cout<<"H "<<userH<<" "<<regH->xmax<<" "<<regH->xmin<<" "<<regH->ymax<<" "<<regH->ymin<<endl;
		cout<<"V "<<userV<<" "<<regV->xmax<<" "<<regV->xmin<<" "<<regV->ymax<<" "<<regV->ymin<<endl;*/
		if ((userH >=k || userV >= k) && (getArea(regH)>=area)){
			if ((userH >=k && userV >= k && getArea(regH)<=getArea(regV)) || (userV < k)){
				return regH;
			}
			else{
				return regV;
			}
		}
		else{
			return query(current->parent,expanded,k,area);
		}
	}
	return this->root->reg;

}
void qtree::remove(qdnode *node,int &expanded){
	expanded++;
	node->users = node->users - 1;
	if (node->parent !=NULL){
		remove(node->parent,expanded);
	}
}
void qtree::update(qdnode *current,point *pnt,int &expanded,int userId,map<int,qdnode*> &hash){
	expanded++;
	if (current->isleaf == true){
		hash[userId] = current;
		current->users = current->users + 1;
		current->stored[userId] = pnt;
		return; 
	}
	else{
		current->users = current->users + 1;
		if (PointInside(pnt,current->NW->reg) == true)
		{
			update(current->NW,pnt,expanded,userId,hash);
		}
		else if (PointInside(pnt,current->NE->reg) == true)
		{
			update(current->NE,pnt,expanded,userId,hash);
		}
		else if (PointInside(pnt,current->SE->reg) == true)
		{
			update(current->SE,pnt,expanded,userId,hash);
		}
		else
		{
			update(current->SW,pnt,expanded,userId,hash);
		}
		return;
	}
}
void qtree::updateUser(point *newL,int &expanded,int userId,map<int,qdnode*> &hash,map<int,point*> &location){
	location[userId] = newL;
	if (PointInside(newL,hash[userId]->reg)){
		hash[userId]->stored[userId] = newL;
	}
	else{
		hash[userId]->stored.erase(userId);
		remove(hash[userId],expanded);
		update(this->root,newL,expanded,userId,hash);
	}
}
double getDistance(double x,double y,point *p){
	return sqrt((x-p->x)*(x-p->x)+(y-p->y)*(y-p->y));
}
bool PointInside(point *p,region *reg){
	if (p->y >= reg->ymin && p->y <= reg->ymax && p->x >= reg->xmin && p->x <= reg->xmax)
	{
		return true;
	}
	else
	{
		return false;
	}
}
vector<point*> candidateList(region *reg,vector<point*> cars){
	point *NW = cars[0];
	point *NE = cars[0];
	point *SE = cars[0];
	point *SW = cars[0];
	for (int i=1;i<cars.size();i++){
		if (getDistance(reg->xmin,reg->ymax,NW)>getDistance(reg->xmin,reg->ymax,cars[i])+0.000001*getDistance(reg->xmin,reg->ymax,cars[i]))
			NW = cars[i];
		if (getDistance(reg->xmax,reg->ymax,NE)>getDistance(reg->xmax,reg->ymax,cars[i])+0.000001*getDistance(reg->xmax,reg->ymax,cars[i]))
			NE = cars[i];
		if (getDistance(reg->xmax,reg->ymin,SE)>getDistance(reg->xmax,reg->ymin,cars[i])+0.000001*getDistance(reg->xmax,reg->ymin,cars[i]))
			SE = cars[i];
		if (getDistance(reg->xmin,reg->ymin,SW)>getDistance(reg->xmin,reg->ymin,cars[i])+0.000001*getDistance(reg->xmin,reg->ymin,cars[i]))
			SW = cars[i];
	}
	
	region *updated ;
	updated = new region;
	updated->ymax = max(NW->y,NE->y);
	updated->xmax = max(NE->x,SE->x);
	updated->ymin = min(SE->y,SW->y);
	updated->xmin = min(NW->x,SW->x);
	vector<point*> cand;
	for (int i=0;i<cars.size();i++){
		if (PointInside(cars[i],updated)==true)
			cand.push_back(cars[i]);
	}
	return cand;
}
int main()
{
	int numOfUsers = 10000;
	std::vector<vector<point*> > dataset_A(10);
	std::vector<vector<pair<point*,int> > > dataset_C(10);
	for (int i=0;i<10;i++){
		dataset_A[i] = vector<point*>(numOfUsers);
		dataset_C[i] = vector<pair<point*,int> >(numOfUsers);
	}
	
	int counter = 0;
	map<int,qdnode*> hash;
	map<int,point*> location;
	map<int,pair<int,double> > profile;
	vector<point*> cars;
	ifstream file1("milan-poi.txt", ios::in);
	double lat;
	double lon;
	for (int i=0;i<1280;i++){
		file1>>lat>>lon;
		point *temp = new point;
		temp->x = lat;
		temp->y = lon;
		cars.push_back(temp);
	}
	file1.close();

	
	ifstream file("milan-users.txt", ios::in);
	for (int i=0;i<numOfUsers;i++){
		for (int j=0;j<10;j++){
			file>>lat>>lon;
			point *temp=new point;
			temp->x = lat;
			temp->y = lon;
			dataset_A[j][i] = temp;
			dataset_C[j][i] = make_pair(temp,i);
		}
	}
	file.close();
	double cell = 500;
	long long total = 0;
	for (int i=0;i<numOfUsers;i++){
		location[i] = dataset_A[0][i];
		int k = rand()%101 + 150;
		double area  = (rand()%10)*(1.0/cell)*(1.0/cell);
		profile[i] = make_pair(k,area);
	}
	qtree *tree4 = new qtree();
	tree4->makeQTree(dataset_A[0],dataset_C[0],1.0/cell,hash);//Quad Tree on Dataset A
	int expandedCloaked = 0;
	int expandedUpdate = 0;
	double cloaked = 0;
	double update = 0;
	clock_t begin = clock();
	for (int i=0;i<1000;i++){
		int userRand = rand()%numOfUsers;
		region *reg = tree4->query(hash[userRand],expandedCloaked,profile[userRand].first,profile[userRand].second);
		//vector<point*> cand = candidateList(reg,cars);
		//total+=cand.size();
	}
	clock_t end = clock();
	cloaked = double(end-begin);
	//cout<<cloaked<<" "<<expandedUpdate<<"  "<<expandedCloaked<<endl;
	for (int k=2;k<10;k++){
		begin = clock();
		for (int j=0;j<numOfUsers;j++){
			tree4->updateUser(dataset_A[k][j],expandedUpdate,j,hash,location);
		}
		end = clock();
		update+= double(end-begin);
		cout<<"After update  "<<update<<"  "<<cloaked<<" "<<expandedCloaked<<endl;
		begin = clock();
		for (int i=0;i<1000;i++){
			int userRand = rand()%numOfUsers;
			region *reg = tree4->query(hash[userRand],expandedCloaked,profile[userRand].first,profile[userRand].second);
			//vector<point*> cand = candidateList(reg,cars);
			//total+=cand.size();
		}
		end = clock();
		cloaked+= double(end-begin);
		cout<<"After query  "<<update<<"  "<<cloaked<<" "<<expandedCloaked<<endl;
	}
	cout<<"Update average time = "<<(update/(numOfUsers*8.0*1.0))<<endl;
	cout<<"Cloaked average time = "<<(cloaked/9000.0)<<"  Cloaked average nodes expansion = "<<(expandedCloaked/9000.0)<<endl;
	//cout<<"Cloaked average cand list = "<<(total/9000.0)<<endl;
    
	//---------------------------------------------------Dataset A-----------------------------------------------
	/*while (counter < 1000)
	{
		point *temp = new point;
		temp->x=(rand()%200)*1.0/(100.0)-1;
		temp->y=(rand()%200)*1.0/(100.0)-1;
		dataset_A.push_back(temp);
		dataset_C.push_back(make_pair(temp,counter));
		location[counter] = temp;
		int k = rand()%100;
		double area  = (rand()%10)*(1.0/cell)*(1.0/cell);
		profile[counter] = make_pair(k,area);
		counter++;
	}*/
	/*while (counter < 10)
	{
		point *temp = new point;
		temp->x=counter-3;
		temp->y=(counter-2.0)/2.0;
		dataset_A.push_back(temp);
		dataset_C.push_back(make_pair(temp,counter));
		location[counter] = temp;
		int k = 3;
		double area = 8;
		profile[counter] = make_pair(k,area);
		counter++;
	}*/
	//---------------------------------------------------Dataset B-----------------------------------------------
	

	
	
	//-----------------------------------------------Building Trees on Three datasets------------------------------------------------------------

	
	/*qtree *tree4 = new qtree();
	tree4->makeQTree(dataset_A[0],dataset_C[0],1.0/cell,hash);//Quad Tree on Dataset A
	
    int expanded = 0;
    point *newL;
    newL = new point;
    newL = dataset_A[9][9999];
    tree4->updateUser(newL,expanded,7,hash,location);
    cout<<" -----------------------------  "<< expanded << "  ------------------------------"<<endl;
    expanded = 0;
    int user = 7;
    region *reg = tree4->query(hash[user],expanded,profile[user].first,profile[user].second);
    cout<<expanded<<" "<<reg->xmax<<" "<<reg->xmin<<" "<<reg->ymax<<" "<<reg->ymin<<endl;
    cout<<profile[user].first<<" "<<profile[user].second<<" "<<getArea(reg)<<endl;
    cout<<hash[user]->reg->xmax<<" "<<hash[user]->reg->xmin<<" "<<hash[user]->reg->ymax<<" "<<hash[user]->reg->ymin<<endl;
    cout<<location[user]->x<<" "<<location[user]->y<<endl;
    cout<<"PRINTING THE USERS"<<endl;*/
    std::cout << "\n----------------------   EXPERIMENT 1(Height of the tree)   -----------------" << std::endl;
	
	std::cout << "Height of Quad Tree: " << tree4->getHeight() << std::endl;
		

	
	
	
	return 0;
}