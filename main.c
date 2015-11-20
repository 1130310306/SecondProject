#include <stdio.h>
#include <stdlib.h>
#define Members struct members
#define Adjacency struct adjacency
#define Group struct group
#define GRAPH struct graph
#define Vi_Members struct vi_members
#define Vk_Members struct vk_members
#define Vk struct vk
#define VMAX 10000
#define SMAX 600
//Step1
Members{
    int name;
    Members* next;
};
Adjacency{
    int name;
    float group_coefficient;
    Adjacency* next;
};
Group{
    int group_weight;
    int label; //tell whether it is a group
    int visit; //tell whether it is in the directed graph
    float direct_graph_max_group_coefficient;
    int direct_graph_name;
    Adjacency* ahead;
    Members* mhead;
    Members* mtail;
    int smallgraph_name;//仅应用于小图划分中
};
GRAPH{
    int graph_vertexsum;int graph_edgesum;
    Group graph_allgroups[VMAX];
    int notfullnumber;
};
//Step1
//Step2
Vi_Members{
    int group_weight;
    Members* mhead;
};
Vk_Members{
    int newname;
    int name;
    int group_weight;
    int L_gvi;
    int R_gvk;
    Vk_Members* next;
};
Vk{
    int group_weight;
    Vk_Members* vkhead;
};
//Step2

int NotIn(Members* mgrouphead,Adjacency* one);
int NameIn(int name,Members* mgrouphead);

int main()
{
    GRAPH agraph;//Step0
    Members* member;
    Adjacency* adjacency;
    FILE* fpvertex;
    FILE* fpedge;
    int i,j,t;
    int v1,v2,ecost;
    int sum_group_coefficient;
    int k;
    float coarsening_degree;
    float partition_degree;
    int Target_Group,Next_Group;//Step1
    int Tempt_Group;
    Adjacency* agrouphead; //The ring's Adjacency* ahead; get
    Members* mgrouphead;
    Members* tempt_mhead;//get
    Members* tempt_mtail;//get
    Members* tempt_mgroup;
    Adjacency* tempt_agroup;
    Adjacency* tempt_adjacency;
    Adjacency* tempt;
    int tempt_group_weight;//get
    float tempt_direct_graph_max_group_coefficient;//get
    int tempt_direct_graph_name;//get
    float LIMIT;
    FILE* fpcoarseningstep;
    int Smallgraph_edges[SMAX][SMAX];//Step2
    Vk avk;
    Vk_Members* tempt_vkmember;
    Vi_Members all_vimembers[SMAX];
    int Smallgraph_Tempt_Group;
    int gmax;//the max g
    int gmax_name;//the coarsening group that has the max g
    int gmax_newname;//the group in the small graph that has the max g
    int gmax_group_weight;//the coarsening group's group_weight that has the max g
    FILE* fppartitionstep;
    int communication_cost;//通信代价大小
    FILE* fpresult;//Step3



    //Step0:原图信息的录入至agraph
    /*原图的初始化*/
    sum_group_coefficient=0;
    for(i=0;i<VMAX;i++){
        agraph.graph_allgroups[i].ahead=NULL;
        agraph.graph_allgroups[i].direct_graph_name=-1;
        agraph.graph_allgroups[i].label=1;
        agraph.graph_allgroups[i].visit=0;
    }
    /*顶点信息的录入*/
    fpvertex=fopen("vertex.txt","r");
    fscanf(fpvertex,"%d",&agraph.graph_vertexsum);
    agraph.notfullnumber=agraph.graph_vertexsum;
    for(i=0;i<agraph.graph_vertexsum;i++){
        fscanf(fpvertex,"%d",&agraph.graph_allgroups[i].group_weight);
        sum_group_coefficient+=agraph.graph_allgroups[i].group_weight;
        member=(Members*)malloc(sizeof(Members)); //set member
        member->name=i;
        member->next=NULL;
        agraph.graph_allgroups[i].mhead=member;
        agraph.graph_allgroups[i].mtail=member;
    }
    /*边信息的录入*/
    fpedge=fopen("edge.txt","r");
    fscanf(fpedge,"%d",&agraph.graph_edgesum);
    for(i=0;i<agraph.graph_edgesum;i++){
        fscanf(fpedge,"%d %d %d",&v1,&v2,&ecost);
        adjacency=(Adjacency*)malloc(sizeof(Adjacency)); //set adjacency v1
        adjacency->name=v2;
        adjacency->group_coefficient=ecost*1.0/agraph.graph_allgroups[v2].group_weight;
        if(agraph.graph_allgroups[v1].ahead==NULL){
           adjacency->next=NULL;
           agraph.graph_allgroups[v1].ahead=adjacency;
           agraph.graph_allgroups[v1].direct_graph_name=v2;
           agraph.graph_allgroups[v1].direct_graph_max_group_coefficient=adjacency->group_coefficient;

        }
        else{
            if(agraph.graph_allgroups[v1].direct_graph_max_group_coefficient<adjacency->group_coefficient){
                agraph.graph_allgroups[v1].direct_graph_name=v2;
                agraph.graph_allgroups[v1].direct_graph_max_group_coefficient=adjacency->group_coefficient;
           }
           adjacency->next=agraph.graph_allgroups[v1].ahead;
           agraph.graph_allgroups[v1].ahead=adjacency;
        }
        adjacency=(Adjacency*)malloc(sizeof(Adjacency)); //set adjacency v2
        adjacency->name=v1;
        adjacency->group_coefficient=ecost*1.0/agraph.graph_allgroups[v1].group_weight;
        if(agraph.graph_allgroups[v2].ahead==NULL){
           adjacency->next=NULL;
           agraph.graph_allgroups[v2].ahead=adjacency;
           agraph.graph_allgroups[v2].direct_graph_name=v1;
           agraph.graph_allgroups[v2].direct_graph_max_group_coefficient=adjacency->group_coefficient;

        }
        else{
            if(agraph.graph_allgroups[v2].direct_graph_max_group_coefficient<adjacency->group_coefficient){
                agraph.graph_allgroups[v2].direct_graph_name=v1;
                agraph.graph_allgroups[v2].direct_graph_max_group_coefficient=adjacency->group_coefficient;
           }
           adjacency->next=agraph.graph_allgroups[v2].ahead;
           agraph.graph_allgroups[v2].ahead=adjacency;
        }
    }
    //Step0:原图信息的录入至agraph
    printf("Paper's Realization based on C programming language：\n");
    printf("Please input the partitions number k:\n");
    scanf("%d",&k);
    printf("Please input the coarsening degree:\n");
    scanf("%f",&coarsening_degree);
    LIMIT=sum_group_coefficient/(k*coarsening_degree);
    printf("LIMIT=%f\n",LIMIT);
    //Step1:粗糙化
    Target_Group=0;
    agraph.graph_allgroups[Target_Group].visit=1;
    fpcoarseningstep=fopen("coarseningstep.txt","w");
    fprintf(fpcoarseningstep,"Coarsening steps:\n");
    while(agraph.notfullnumber>=2){
        fprintf(fpcoarseningstep,"\nnotfullnumber=%d\n",agraph.notfullnumber);
        fprintf(fpcoarseningstep,"targertgroup -> %d\n",Target_Group);
        Next_Group=agraph.graph_allgroups[Target_Group].direct_graph_name;
        //fprintf(fpcoarseningstep,"nextgroup -> %d\n",Next_Group);
        if(agraph.graph_allgroups[Next_Group].visit==1){ //find out a ring
            fprintf(fpcoarseningstep,"find a ring!\n");
            printf("find a ring!\n");
            //Step1.0:merge the groups on the ring
            fprintf(fpcoarseningstep,"      1.merge the ring\n");
            tempt_mhead=agraph.graph_allgroups[Next_Group].mhead; //get all member groups of the directed ring
            tempt_mtail=agraph.graph_allgroups[Next_Group].mtail;
            member=(Members*)malloc(sizeof(Members));
            member->name=Next_Group;
            member->next=NULL;
            mgrouphead=member;
            tempt_group_weight = agraph.graph_allgroups[Next_Group].group_weight; //get the sum weight of the ring
            Tempt_Group=Next_Group;
            //printf("\n%d*****!!!!  ",Next_Group);
            while(Tempt_Group!=Target_Group){
                Tempt_Group=agraph.graph_allgroups[Tempt_Group].direct_graph_name;
                //printf("  %d*****!!!!  ",Tempt_Group);
                tempt_mtail->next=agraph.graph_allgroups[Tempt_Group].mhead;
                tempt_mtail=agraph.graph_allgroups[Tempt_Group].mtail;
                member=(Members*)malloc(sizeof(Members));
                member->name=Tempt_Group;
                member->next=mgrouphead;
                mgrouphead=member;
                tempt_group_weight += agraph.graph_allgroups[Tempt_Group].group_weight;
                agraph.graph_allgroups[Tempt_Group].label=0; //not a group
                agraph.notfullnumber--;
            }

            tempt_mgroup=mgrouphead;  //start merge the group members on the ring
            agrouphead=NULL;
            while(tempt_mgroup!=NULL){
                tempt_agroup=agraph.graph_allgroups[tempt_mgroup->name].ahead;
                while(tempt_agroup!=NULL){
                    if(NotIn(mgrouphead,tempt_agroup)==1){//not in
                        tempt_adjacency=agrouphead;
                        while(tempt_adjacency!=NULL){  //get the ring's adjacency
                            if(tempt_adjacency->name==tempt_agroup->name){ //have
                                tempt_adjacency->group_coefficient += tempt_agroup->group_coefficient;
                                if(tempt_adjacency->group_coefficient>tempt_direct_graph_max_group_coefficient&&agraph.graph_allgroups[tempt_agroup->name].label==1){
                                    tempt_direct_graph_name=tempt_agroup->name;
                                    tempt_direct_graph_max_group_coefficient=tempt_adjacency->group_coefficient;
                                }
                                break;
                            }
                            tempt_adjacency=tempt_adjacency->next;
                        }
                        if(tempt_adjacency==NULL){ //not have
                            adjacency=(Adjacency*)malloc(sizeof(Adjacency));
                            adjacency->name=tempt_agroup->name;
                            adjacency->group_coefficient=tempt_agroup->group_coefficient;
                            adjacency->next=agrouphead;
                            agrouphead=adjacency;
                            if(agrouphead->next==NULL){
                                if(agraph.graph_allgroups[tempt_agroup->name].label==1){
                                    tempt_direct_graph_name=tempt_agroup->name;
                                    tempt_direct_graph_max_group_coefficient=adjacency->group_coefficient;
                                }
                                else{
                                    tempt_direct_graph_name=-1;
                                    tempt_direct_graph_max_group_coefficient=-1;
                                }
                            }
                            else{
                                if(adjacency->group_coefficient>tempt_direct_graph_max_group_coefficient&&agraph.graph_allgroups[tempt_agroup->name].label==1){
                                    tempt_direct_graph_name=adjacency->name;
                                    tempt_direct_graph_max_group_coefficient=adjacency->group_coefficient;
                                }
                            }
                        }
                    }
                    tempt_agroup=tempt_agroup->next;
                }
                tempt_mgroup=tempt_mgroup->next;
            }
            agraph.graph_allgroups[Next_Group].label=1; //is a group->merge all data into the group Next_Group
            agraph.graph_allgroups[Next_Group].group_weight=tempt_group_weight;
            agraph.graph_allgroups[Next_Group].ahead=agrouphead;
            agraph.graph_allgroups[Next_Group].direct_graph_max_group_coefficient=tempt_direct_graph_max_group_coefficient;
            agraph.graph_allgroups[Next_Group].direct_graph_name=tempt_direct_graph_name;
            agraph.graph_allgroups[Next_Group].mhead=tempt_mhead;
            agraph.graph_allgroups[Next_Group].mtail=tempt_mtail;
            //Step1.0:merge the groups on the ring
            //Step1.1:change the direct_graph_name and direct_graph_group_coefficient of the adjacent groups related to the ring
            fprintf(fpcoarseningstep,"      1.update the data\n");
            tempt_agroup=agraph.graph_allgroups[Next_Group].ahead;
            while(tempt_agroup!=NULL){
                Tempt_Group=tempt_agroup->name;//change the max (<=LIMIT)
                tempt_adjacency=agraph.graph_allgroups[Tempt_Group].ahead;//delete the groups related to the ring
                while(tempt_adjacency->next!=NULL){
                    if(NotIn(mgrouphead,tempt_adjacency->next)==0){//in
                        tempt_adjacency->next=tempt_adjacency->next->next;
                    }
                    else{
                        tempt_adjacency=tempt_adjacency->next;
                    }
                }
                if(NotIn(mgrouphead,agraph.graph_allgroups[Tempt_Group].ahead)==0){//in
                    agraph.graph_allgroups[Tempt_Group].ahead=agraph.graph_allgroups[Tempt_Group].ahead->next;
                }
                adjacency=(Adjacency*)malloc(sizeof(Adjacency));
                adjacency->name=Next_Group;
                adjacency->group_coefficient=tempt_agroup->group_coefficient*agraph.graph_allgroups[Tempt_Group].group_weight/agraph.graph_allgroups[Next_Group].group_weight;
                adjacency->next=agraph.graph_allgroups[Tempt_Group].ahead;
                agraph.graph_allgroups[Tempt_Group].ahead=adjacency;
                tempt_agroup=tempt_agroup->next;
            }
            if(agraph.graph_allgroups[Next_Group].group_weight>=LIMIT){ //change the max (>LIMIT)
                tempt_agroup=agraph.graph_allgroups[Next_Group].ahead;
                while(tempt_agroup!=NULL){
                    Tempt_Group=tempt_agroup->name;//change the max
                    if(agraph.graph_allgroups[Tempt_Group].label==1){
                        if(NameIn(agraph.graph_allgroups[Tempt_Group].direct_graph_name,mgrouphead)==1){//in
                            agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient=-1;
                            agraph.graph_allgroups[Tempt_Group].direct_graph_name=-1;
                            tempt_adjacency=agraph.graph_allgroups[Tempt_Group].ahead;
                            while(tempt_adjacency!=NULL){
                                if(tempt_adjacency->name!=Next_Group&&agraph.graph_allgroups[tempt_adjacency->name].label==1){
                                    if(tempt_adjacency->group_coefficient>agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient){
                                        agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient=tempt_adjacency->group_coefficient;
                                        agraph.graph_allgroups[Tempt_Group].direct_graph_name=tempt_adjacency->name;
                                    }
                                }
                                tempt_adjacency=tempt_adjacency->next;
                            }
                        }
                    }
                    tempt_agroup=tempt_agroup->next;
                }
            }
            else{
                tempt_agroup=agraph.graph_allgroups[Next_Group].ahead;
                while(tempt_agroup!=NULL){
                    Tempt_Group=tempt_agroup->name;
                    if(agraph.graph_allgroups[Tempt_Group].label==1){
                        if(agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient<=tempt_agroup->group_coefficient*agraph.graph_allgroups[Tempt_Group].group_weight/agraph.graph_allgroups[Next_Group].group_weight){
                            agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient=tempt_agroup->group_coefficient*agraph.graph_allgroups[Tempt_Group].group_weight/agraph.graph_allgroups[Next_Group].group_weight;
                            agraph.graph_allgroups[Tempt_Group].direct_graph_name=Next_Group;
                        }
                        else{
                            if(NameIn(agraph.graph_allgroups[Tempt_Group].direct_graph_name,mgrouphead)==1){//in
                                agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient=-1;
                                agraph.graph_allgroups[Tempt_Group].direct_graph_name=-1;
                                tempt_adjacency=agraph.graph_allgroups[Tempt_Group].ahead;
                                while(tempt_adjacency!=NULL){
                                    if(agraph.graph_allgroups[tempt_adjacency->name].label==1){
                                        if(tempt_adjacency->group_coefficient>agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient){
                                            agraph.graph_allgroups[Tempt_Group].direct_graph_max_group_coefficient=tempt_adjacency->group_coefficient;
                                            agraph.graph_allgroups[Tempt_Group].direct_graph_name=tempt_adjacency->name;
                                        }
                                    }
                                    tempt_adjacency=tempt_adjacency->next;
                                }
                            }
                        }
                    }
                    tempt_agroup=tempt_agroup->next;
                }
            }
            //Step1.1:change the direct_graph_name and direct_graph_group_coefficient of the adjacent groups related to the ring
            //Step1.2:tell whether this merged group is up to the limit size, if so stop coarsening this group
            for(i=0;i<VMAX;i++){
                agraph.graph_allgroups[i].visit=0;
            }
            if(agraph.graph_allgroups[Next_Group].group_weight>=LIMIT){
                fprintf(fpcoarseningstep,"The ring is up to limit!!!\n");
                agraph.graph_allgroups[Next_Group].label=2; //stop merging this ring
                agraph.notfullnumber--;
                agraph.graph_allgroups[Next_Group].direct_graph_name=-1;
                agraph.graph_allgroups[Next_Group].direct_graph_max_group_coefficient=-1;
                tempt_agroup=agraph.graph_allgroups[Next_Group].ahead;
                while(tempt_agroup->next!=NULL){
                    Tempt_Group=tempt_agroup->next->name;//change the max
                    if(agraph.graph_allgroups[Tempt_Group].direct_graph_name==-1&&agraph.graph_allgroups[Tempt_Group].label==1){ //join the ring
                        agraph.graph_allgroups[Tempt_Group].label=2;
                        agraph.notfullnumber--;
                        fprintf(fpcoarseningstep,"!!!   ADD   !!!\n");
                    }
                    else{
                        tempt_agroup=tempt_agroup->next;
                    }
                }
                Tempt_Group=agraph.graph_allgroups[Next_Group].ahead->name;
                if(agraph.graph_allgroups[Tempt_Group].direct_graph_name==-1&&agraph.graph_allgroups[Tempt_Group].label==1){ //join the ring
                    agraph.graph_allgroups[Tempt_Group].label=2;
                    fprintf(fpcoarseningstep,"!!!   ADD   !!!\n");
                    agraph.notfullnumber--;
                }
                for(i=0;i<VMAX;i++){
                    if(agraph.graph_allgroups[i].label==1){
                        Target_Group=i;
                        agraph.graph_allgroups[Target_Group].visit=1;
                        break;
                    }
                }
            }
            else{
                fprintf(fpcoarseningstep,"The ring is not up to limit!!!\n");
                agraph.graph_allgroups[Next_Group].visit=1;
                Target_Group=Next_Group;
            }
            printf("!!!!finish merging the ring!!!!\n");
            printf("\nnotfullnumber=%d\n",agraph.notfullnumber);
        //Step1.2:tell whether this merged group is up to the limit size, if so stop coarsening this group
        }
        else{ //no ring
            fprintf(fpcoarseningstep,"No ring!\n");
            agraph.graph_allgroups[Next_Group].visit=1;
            Target_Group=Next_Group;
        }
        //显示graph中所有信息
        /*
        fprintf(fpcoarseningstep,"\nshow the graph!!!\n");
        fprintf(fpcoarseningstep,"graph_allgroups[]:\n");
        for(i=0;i<agraph.graph_vertexsum;i++){
            if(agraph.graph_allgroups[i].label!=0){
                fprintf(fpcoarseningstep,"name:%d,group_weight:%d\n",i,agraph.graph_allgroups[i].group_weight);
                fprintf(fpcoarseningstep,"visit:%d,label:%d,direct_graph_max_group_coefficient:%.1f,direct_graph_name:%d\n",agraph.graph_allgroups[i].visit,agraph.graph_allgroups[i].label,agraph.graph_allgroups[i].direct_graph_max_group_coefficient,agraph.graph_allgroups[i].direct_graph_name);
                fprintf(fpcoarseningstep,"members are:\n");
                member=agraph.graph_allgroups[i].mhead;
                while(member!=NULL){
                    fprintf(fpcoarseningstep,"name:%d\n",member->name);
                    member=member->next;
                }

                fprintf(fpcoarseningstep,"adjacency are:\n");
                adjacency=agraph.graph_allgroups[i].ahead;
                while(adjacency!=NULL){
                    fprintf(fpcoarseningstep,"name:%d,group_coefficient:%f\n",adjacency->name,adjacency->group_coefficient);
                    adjacency=adjacency->next;
                }
            }
        }*/
        //显示graph中所有信息
    }
        fprintf(fpcoarseningstep,"\nshow the graph!!!\n");
        fprintf(fpcoarseningstep,"graph_allgroups[]:\n");
        for(i=0;i<agraph.graph_vertexsum;i++){
            if(agraph.graph_allgroups[i].label!=0){
                fprintf(fpcoarseningstep,"name:%d,group_weight:%d\n",i,agraph.graph_allgroups[i].group_weight);
                fprintf(fpcoarseningstep,"visit:%d,label:%d,direct_graph_max_group_coefficient:%.1f,direct_graph_name:%d\n",agraph.graph_allgroups[i].visit,agraph.graph_allgroups[i].label,agraph.graph_allgroups[i].direct_graph_max_group_coefficient,agraph.graph_allgroups[i].direct_graph_name);
                fprintf(fpcoarseningstep,"members are:\n");
                member=agraph.graph_allgroups[i].mhead;
                while(member!=NULL){
                    fprintf(fpcoarseningstep,"name:%d\n",member->name);
                    member=member->next;
                }

                fprintf(fpcoarseningstep,"adjacency are:\n");
                adjacency=agraph.graph_allgroups[i].ahead;
                while(adjacency!=NULL){
                    fprintf(fpcoarseningstep,"name:%d,group_coefficient:%f\n",adjacency->name,adjacency->group_coefficient);
                    adjacency=adjacency->next;
                }
            }
        }
    fclose(fpcoarseningstep);
    //Step1:粗糙化
    //Step2:小图划分
    //Step2.0:小数据图相关数据的录入（构建和初始化二维边表，构建Vk链表和初始化Vi各成员和重新设定粗糙化后的小图中各团的名称smallgraph_name）
    for(i=0;i<k;i++){ //初始化Vi各成员
        all_vimembers[i].group_weight=0;
        all_vimembers[i].mhead=NULL;
    }
    t=0;//重新设定粗糙化后的小图中各团的名称smallgraph_name
    for(i=0;i<agraph.graph_vertexsum;i++){
        if(agraph.graph_allgroups[i].label!=0){
            agraph.graph_allgroups[i].smallgraph_name=t;
            t++;
        }
    }
    for(i=0;i<SMAX;i++){//初始化二维边表
        for(j=0;j<SMAX;j++){
            Smallgraph_edges[i][j]=0;
        }
    }
    avk.vkhead=NULL;//构建二维边表，Vk链表
    avk.group_weight=0;
    for(i=0;i<agraph.graph_vertexsum;i++){
        if(agraph.graph_allgroups[i].label!=0){
            tempt_vkmember=(Vk_Members*)malloc(sizeof(Vk_Members));
            tempt_vkmember->newname=agraph.graph_allgroups[i].smallgraph_name;
            tempt_vkmember->name=i;
            tempt_vkmember->group_weight=agraph.graph_allgroups[i].group_weight;
            avk.group_weight+=tempt_vkmember->group_weight;
            tempt_vkmember->L_gvi=0;
            tempt_vkmember->R_gvk=0;
            tempt_adjacency=agraph.graph_allgroups[i].ahead;
            while(tempt_adjacency!=NULL){
                Smallgraph_Tempt_Group=agraph.graph_allgroups[tempt_adjacency->name].smallgraph_name;
                Smallgraph_edges[tempt_vkmember->newname][Smallgraph_Tempt_Group]=tempt_adjacency->group_coefficient*agraph.graph_allgroups[tempt_adjacency->name].group_weight;
                tempt_vkmember->R_gvk+=Smallgraph_edges[tempt_vkmember->newname][Smallgraph_Tempt_Group];
                tempt_adjacency=tempt_adjacency->next;
            }
            tempt_vkmember->next=avk.vkhead;
            avk.vkhead=tempt_vkmember;
        }
    }
    //Step2.0:小数据图相关数据的录入（构建和初始化二维边表，构建Vk链表和初始化Vi各成员和重新设定粗糙化后的小图中各团的名称smallgraph_name）
    //Step2.1:小数据图k划分开始
    printf("Please input the partition degree:\n");
    scanf("%f",&partition_degree);
    fppartitionstep=fopen("partitionstep.txt","w");
    communication_cost=0;
    for(i=0;i<k-1;i++){
        fprintf(fppartitionstep,"\nNow is V%d:\n",i);
        while(all_vimembers[i].group_weight<sum_group_coefficient/k*partition_degree&&avk.group_weight>=sum_group_coefficient/k){
            tempt_vkmember=avk.vkhead;//find out the max g(its name and its newname in the small graph)
            gmax=tempt_vkmember->L_gvi-tempt_vkmember->R_gvk;
            gmax_name=tempt_vkmember->name;
            gmax_newname=tempt_vkmember->newname;
            gmax_group_weight=tempt_vkmember->group_weight;
            while(tempt_vkmember->next!=NULL){
                if((tempt_vkmember->next->L_gvi-tempt_vkmember->next->R_gvk)>gmax){
                    gmax=tempt_vkmember->next->L_gvi-tempt_vkmember->next->R_gvk;
                    gmax_name=tempt_vkmember->next->name;
                    gmax_newname=tempt_vkmember->next->newname;
                    gmax_group_weight=tempt_vkmember->next->group_weight;
                }
                tempt_vkmember=tempt_vkmember->next;
            }
            communication_cost += (-1)*gmax;
            fprintf(fppartitionstep,"\ngmax=%d,gmax_name=%d,gmax_newname=%d,gmax_group_weight=%d\n",gmax,gmax_name,gmax_newname,gmax_group_weight);
            printf("\ngmax=%d,gmax_name=%d,gmax_newname=%d,gmax_group_weight=%d\n",gmax,gmax_name,gmax_newname,gmax_group_weight);
            all_vimembers[i].group_weight+=gmax_group_weight;
            member=(Members*)malloc(sizeof(Members));
            member->name=gmax_name;
            member->next=all_vimembers[i].mhead;
            all_vimembers[i].mhead=member;
            fprintf(fppartitionstep,"V%d add a group,its name in the coarsening graph is:%d\n",i,member->name);
            tempt_vkmember=avk.vkhead;//update the data after finding out the max g in vk
            avk.group_weight-=gmax_group_weight;
            fprintf(fppartitionstep,"V%d 's group weight is: %d ,Vk 's group weight is: %d\n",i,all_vimembers[i].group_weight,avk.group_weight);
            printf("V%d 's group weight is: %d ,Vk 's group weight is: %d\n",i,all_vimembers[i].group_weight,avk.group_weight);
            while(tempt_vkmember->next!=NULL){
                if(tempt_vkmember->next->newname==gmax_newname){
                    tempt_vkmember->next=tempt_vkmember->next->next;
                }
                else{
                    tempt_vkmember->next->L_gvi+=Smallgraph_edges[gmax_newname][tempt_vkmember->next->newname];
                    tempt_vkmember->next->R_gvk-=Smallgraph_edges[gmax_newname][tempt_vkmember->next->newname];
                    tempt_vkmember=tempt_vkmember->next;
                }
            }
            if(avk.vkhead->newname==gmax_newname){
               avk.vkhead=avk.vkhead->next;
            }
            else{
                avk.vkhead->L_gvi+=Smallgraph_edges[gmax_newname][avk.vkhead->newname];
                avk.vkhead->R_gvk-=Smallgraph_edges[gmax_newname][avk.vkhead->newname];
            }
        }
        tempt_vkmember=avk.vkhead;//construct the next vi
        while(tempt_vkmember!=NULL){
            tempt_vkmember->L_gvi=0;
            tempt_vkmember=tempt_vkmember->next;
        }
    }
    tempt_vkmember=avk.vkhead;
    fprintf(fppartitionstep,"\n\nThe V%d's left groups are:\n\n",k-1);
    printf("\n\nThe V%d's left groups are:\n\n",k-1);
    while(tempt_vkmember!=NULL){
        all_vimembers[k-1].group_weight+=tempt_vkmember->group_weight;
        member=(Members*)malloc(sizeof(Members));
        member->name=tempt_vkmember->name;
        member->next=all_vimembers[k-1].mhead;
        all_vimembers[k-1].mhead=member;
        tempt_vkmember=tempt_vkmember->next;
        fprintf(fppartitionstep,"V%d left a group,its name in the coarsening graph is:%d\n",k-1,member->name);
        printf("V%d left a group,its name in the coarsening graph is:%d\n",k-1,member->name);
    }
    fclose(fppartitionstep);
    //Step2.1:小数据图k划分开始
    //Step2:小图划分
    //Step3:逆粗糙化
    //将划分结果映射回原图并写到partition.txt中
    fpresult=fopen("result.txt","w");
    fprintf(fpresult,"\nshow the result (K Partition result) !!!\n");
    for(i=0;i<k;i++){ //显示partition的所有信息
        fprintf(fpresult,"\n\npartition_Vi:%d,sum_weight:%d\n",i,all_vimembers[i].group_weight);
        fprintf(fpresult,"\nthe members in the coarsening graph -> origional graph are:\n");
        member=all_vimembers[i].mhead;
        while(member!=NULL){
            fprintf(fpresult,"\n%d -> ",member->name);
            tempt_mhead=agraph.graph_allgroups[member->name].mhead;
            while(tempt_mhead!=NULL){
                fprintf(fpresult,"%d ",tempt_mhead->name);
                tempt_mhead=tempt_mhead->next;
            }
            fprintf(fpresult,"\n");
            member=member->next;
        }
    }
    fprintf(fpresult,"\n\n\nThe Commumition Cost of the Final Result is : %d \n",communication_cost);
    fclose(fpresult);
    //将划分结果映射回原图并写到partition.txt中
    //Step3:逆粗糙化

/*
    //显示graph中所有信息
    printf("\nshow the graph!!!\n");
    printf("\ngraph_vertexsum,graph_edgesum:\n");
    printf("%d,%d\n",agraph.graph_vertexsum,agraph.graph_edgesum);
    printf("\ngraph_allgroups[]:\n");
    for(i=0;i<agraph.graph_vertexsum;i++){
        printf("name:%d,group_weight:%d\n",i,agraph.graph_allgroups[i].group_weight);
        printf("visit:%d,label:%d,direct_graph_max_group_coefficient:%.1f,direct_graph_name:%d\n",agraph.graph_allgroups[i].visit,agraph.graph_allgroups[i].label,agraph.graph_allgroups[i].direct_graph_max_group_coefficient,agraph.graph_allgroups[i].direct_graph_name);
        printf("members:\n");
        member=agraph.graph_allgroups[i].mhead;
        while(member!=NULL){
            printf("name:%d\n",member->name);
            member=member->next;
        }
        printf("adjacency:\n");
        adjacency=agraph.graph_allgroups[i].ahead;
        while(adjacency!=NULL){
            printf("name:%d,group_coefficient:%.1f\n",adjacency->name,adjacency->group_coefficient);
            adjacency=adjacency->next;
        }
    }
*/
    return 0;
}




int NotIn(Members* mgrouphead,Adjacency* one){
    Members* tempt;
    tempt=mgrouphead;
    while(tempt!=NULL){
        if(tempt->name==one->name){
            return 0;//in
        }
        tempt=tempt->next;
    }
    return 1;//not in
}

int NameIn(int name,Members* mgrouphead){
    Members* tempt;
    tempt=mgrouphead;
    while(tempt!=NULL){
        if(tempt->name==name){
            return 1;//in
        }
        tempt=tempt->next;
    }
    return 0;//not in
}
