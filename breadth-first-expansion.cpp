std::pair<std::vector<int>, std::vector<int>> GetDependencies(const std::vector<GridNode> &nodes){
 std::vector<int> deps(nodes.size(),0); //How many upstream dependencies each node has
 std::vector<int> ordering;             //Order in which nodes must be processed to ensure causal independence
 std::vector<int> levels;               //Set of half-open intervals defining parallelisable sets of nodes

 ordering.reserve(nodes.size());        //Ordering will eventually include every node

 //q will be used to generate a breadth-first traversal of the flow graph
 std::queue<int> q;                    

 //Add an initial marker so each level is defined by a pair of integers
 //indicating a half-open interval [start,end).
 levels.emplace(0);

 //Loop through all nodes figuring out how many upstream dependencies they each
 //have
 for(unsigned int i=0;i<nodes.size();i++)
   deps[nodes[i].downStreamNode]++;

 //Find those nodes with no upstream dependencies
 for(unsigned int i=0;i<deps.size();i++)
   if(deps[i]==0)                       //Node has no upstream dependencies, so it's now part of the frontier of the BFS
     q.emplace(i);                      //Node was a local maxima, so it is part of the initial frontier of the BFS
 q.emplace(-1);                         //Special marker that indicates the end of a level
   
 while(true){                           //Break condition is below
   const auto c = q.front();            //Index of current node

   if(c==-1){                           //This is a level marker, so we need to make a note
     levels.emplace(ordering.size());   //Make the note
     if(q.empty())                      //If the queue is empty after eliminating the level marker
       break;                           //then we've reached the end.
     q.emplace(-1);                     //Put a new marker on the end of the queue
     continue;
   }

   const auto &cn = nodes[c];           //The current node itself
   const auto dn  = cn.downStreamNode;  //Index of the downstream node
   ordering.emplace(c);                 //Place the node in the ordering
   if(--deps[dn]==0)                    //Downstream neighbour has one fewer upstream dependency. Add it to the queue if there are no upstream dependencies.
     q.emplace(cn.downStreamNode);      //Place the downstream neighbour into the queue
 }

 //Add a final marker so each level is defined by a pair of integers indicating
 //a half-open interval [start,end).
 levels.emplace(ordering.size());

 //Vector allocation is exponential, so we end up using more space than we
 //need. This shrinks the vectors to their minimum necessary length, i.e. there
 //is no reserved space.
 ordering.shrink_to_fit();
 levels.shrink_to_fit();

 return std::make_pair(ordering,levels);
}
