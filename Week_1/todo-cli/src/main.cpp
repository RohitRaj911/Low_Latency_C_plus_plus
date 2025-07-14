#include<bits/stdc++.h>
using namespace std;

void showMenu(){
    cout << "\n--- To-Do List Menu ---\n";
    cout << "1. Add Task\n2. View Tasks\n3. Delete Task\n4. Exit\nChoose: ";
}
int main(){
    vector<string>tasks;
    
    while(true){
         showMenu();
         int choice;
         cin>>choice;
         cin.ignore();
        if(choice == 1){
            cout<<"Enter Task: \n";
            string task;
            cin>>task;
            tasks.push_back(task);
            cout<<"Task Added\n";
        }else if(choice ==2){
            cout<<"Your Tasks are :\n";
            for(int i=0;i<tasks.size();i++){
                cout<<tasks[i]<<endl;
            }
        }else if(choice==3){
            string delTask;
            cout<<"Enter your tasks to delete : ";
            cin>>delTask;
            for(int i=0;i<tasks.size();i++){
                if(tasks[i]==delTask){
                    tasks.erase(tasks.begin() + i);
                    cout<<"Your Task is deleted.\n";
                }
            }
            cout<<"There is no task to delete, choose any other.\n";
        }else if(choice==4){
            break;
        }else{
            cout<<"Error!!!!, Invalid Choice\n";
        }
    }
    return 0;
}