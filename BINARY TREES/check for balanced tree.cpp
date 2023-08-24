class Solution{
     public:
     pair<bool,int>isBalancedFast(node* root);
       //base case
        if(root == NULL){
            pair<bool,int> p = make_pair(true,0);
          return p;
        }
        pair<int,int> left = isBalancedFast(root->left);
        pair<int,int>right = isBalancedFast(root->right);
        bool leftAns = left.first;
        bool rightAns = right.first;
        bool diff = abs (left.second - right.second)<=1;
//abs-absolute (it will act as modulus
        pair<bool,int>ans;
        ans.second = max(left.second, right.second)+1;
        if(leftAns && rightAns && diff){
            ans.first=true;
        }
        else{
             ans.first = false;
        }
        return ans;
} 
bool isBalanced(Node *root) {
     return isBalancedFast(root).first;
]
