#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

/**********************************************************************************************************************/

void create_chart(void);            //create m chart & stores m terms in m_chart(in binary) & in m_term(in decimal)
int compare(void);                  //compares each m terms
void draw_chart(void);              //draw the current chart
int* decode(int,int**);             //decodes dashes and return combinations
void find_PI(int);                  //finds PI
void print_PI(void);                //prints PI
void create_PI_table(void);         //create a PI table with crosses
void draw_PI_table(void);           //draw PI table with crosses
void print_essential_PI(void);      //prints essential PI
void create_reduced_PI_table(void); //create a reduced PI chart if all m terms are not covered
void draw_reduced_PI_table(void);   //draw reduced PI table
void find_non_essential(void);      //finds non essential
void print_non_essential(void);     //prints non essentials
void solution(void);                //gives the final solution
void cover(int,int**);              //gives the solution of minimum cover
void rand_no(void);                     //generate random numbers

/**********************************************************************************************************************/

int no_variable;                  //no. of variables
int no_m_term;                    //no. of m terms
int* m_term;                      //stores m terms in decimal
int** chart;                      //stores m terms in binary
int chart_size;                   //current chart size
int** holder;                     //temporary holder
int* check_box;                   //holds information of m terms which are going to next step
int** PI;                         //prime implicants
int no_PI;                        //no of PI
int* decoded;                     //stores decoded value
int** PI_table;                   //PI table
int* covered;                     //is any m terms covered or not
int* essential;                   //is any PI a essential or not
int allcovered;                   //is all m terms are covered or not
int** reduced_PI_table;           //reduced PI table
int no_r_PI;                      //no of PI in reduced PI table
int no_r_m_term;                  //no of m terms in reduced PI table
int* r_m_term;                    //reduced min terms
int** r_PI;                       //reduced PI
int* non_essential;               //non essential
int reduce=0;                     //counts the process of reduction
int* random_no;                   //holding random numbers

/**********************************************************************************************************************/

int main()
{
    long maximum_no_m_terms;
    char choice='y';

    printf("---------------------------------------------------------------------------\n");
    printf("\n\tAssignment : 1\tName : Somenath Maji\t Roll no : 13/CS/05\n");
    printf("\n\tQuine McClauskey Logic Minimization\n");
    printf("\n\tDesigned and implemented by : Somenath Maji\n\n");
    printf("----------------------------------------------------------------------------\n\n");

    while(choice=='Y' || choice=='y')
    {
        top:
        printf("no. of variables :");
        scanf("%d",&no_variable);
        printf("no. of minterms :");
        scanf("%d",&no_m_term);
        maximum_no_m_terms=pow(2,no_variable);
        if(no_m_term > maximum_no_m_terms || no_m_term==0)
        {
            if(no_m_term>maximum_no_m_terms)
                printf("\nERROR : can't hold %d minterms in 2^%d \n",no_m_term,no_variable);
            if(no_m_term==0)
                printf("\nERROR : can't minimize zero min terms\n");
            printf("\ntry again\n\n");
            goto top;
        }
        chart_size=no_m_term;           //initially chart size is equals to no of meanterms
        no_PI=0;                        //initially there are no prime implicants
        allcovered=0;                   //initially all m terms are not covered
        reduce=0;                       //for every time it should initialize at zero

        create_chart();                 //1.comparing min terms
        draw_chart();                   //2.drawing each table
        print_PI();                     //3.finding PI
        create_PI_table();              //4.creating a PI table
        draw_PI_table();                //5.drawing PI table
        print_essential_PI();           //6.looking for essential PI
        if(allcovered)                  //if all min terms are covered then no need to find non essentials
        {
            printf("All min terms are covered with essential prime implicants.\n\n");
        }
        else                            //non essentials are there..
        {
            printf("reducing the PI table...\n\n");
            find_non_essential();
            printf("after doing row and column dominance operation...\n\n");
            print_non_essential();
        }
        solution();                     //giving the final solution

        free(m_term);                   //freeing memory
        free(chart);
        free(holder);
        free(check_box);
        free(PI);
        free(decoded);
        free(PI_table);
        free(covered);
        free(r_m_term);
        free(r_PI);
        free(non_essential);
        free(random_no);
        printf("\nDo you want to continue (y/n) :\n");
        choice=getch();
        printf("\n");
    }
    return 0;
}

/**********************************************************************************************************************/

void create_chart(void)
{
    int i,j;
    int x;
    long strength=pow(2,no_variable)-1;
    chart=malloc(no_m_term*sizeof(int *));
    for(i=0;i<no_m_term;i++)
        chart[i]=malloc(no_variable*sizeof(int));

    m_term=malloc(no_m_term*sizeof(int));
    printf("\n%d minterms chosen randomly are :\n",no_m_term);
    rand_no();

    for(i=0;i<no_m_term;i++)
    {
        for(j=i;j<no_m_term;j++)
        {
            if(random_no[i]>random_no[j])
            {
                x=random_no[i];
                random_no[i]=random_no[j];
                random_no[j]=x;
            }
        }
    }

    for(i=0;i<no_m_term;i++)
        printf("%d ",random_no[i]);
        printf("\n");
    for(i=0;i<no_m_term;i++)
    {

        x=random_no[i];
        m_term[i]=x;
        for(j=no_variable-1;j>=0;j--)
        {
            chart[i][j]=x%2;
            x/=2;
        }
    }
    printf("\n");
}

/**********************************************************************************************************************/

int compare(void)
{
    int i,j,k;
    int next;
    int difference;
    int duplicate;
    int position;
    int further=0;
    int** temp_chart;

    int combination=(chart_size*(chart_size-1))/2;            //no of possible combination

    holder=malloc(combination*sizeof(int *));
    for(i=0;i<combination;i++)
        holder[i]=malloc(no_variable*sizeof(int));
    int holder_size=0;

    check_box=malloc(chart_size*sizeof(int));
    for(i=0;i<chart_size;i++)
        check_box[i]=0;                                       //initially no combinations

    for(i=0;i<chart_size;i++)
    {
        for(next=i+1;next<chart_size;next++)                    //compare to next m term
        {
            difference=0;                                     //before compareing bit difference is 0
            for(j=0;j<no_variable;j++)
            {
                if(chart[next][j] != chart[i][j])
                {
                    difference++;
                    position=j;
                }
            }
            if(difference==1)                                //if bit difference is 1
            {
                check_box[i]=1;
                check_box[next]=1;
                for(j=0;j<no_variable;j++)
                {
                    holder[holder_size][j]=chart[i][j];
                }
                holder[holder_size][position]=-1;
                holder_size++;
            }
        }
    }

    for(i=0;i<chart_size;i++)
    {
        if(check_box[i]==1)
            further++;
        else
            find_PI(i);
    }

    free(chart);

    temp_chart=malloc(holder_size*sizeof(int *));                 //creating a new temporary chart
    for(i=0;i<holder_size;i++)                                    //temp chart holds only non duplicates
        temp_chart[i]=malloc(no_variable*sizeof(int));
    chart_size=0;

    for(i=0;i<holder_size;i++)                                   //eliminating duplicates
    {
        for(k=0;k<chart_size;k++)
        {
            duplicate=0;
            for(j=0;j<no_variable;j++)
            {
                if(temp_chart[k][j] == holder[i][j])
                    duplicate++;
            }
            if(duplicate==no_variable)
                goto check;
        }
        check:
        if(duplicate!=no_variable)
        {
            for(j=0;j<no_variable;j++)
                temp_chart[chart_size][j]=holder[i][j];
            chart_size++;
        }

    }

    chart=malloc(chart_size*sizeof(int *));                 //creating a new chart
    for(i=0;i<chart_size;i++)
        chart[i]=malloc(no_variable*sizeof(int));

    for(i=0;i<chart_size;i++)                              //copying temp chart in final chart
        for(j=0;j<no_variable;j++)
            chart[i][j]=temp_chart[i][j];

    free(holder);                                       //don't need it further
    free(temp_chart);                                   //don't need it further
    free(check_box);                                    //don't need it further

    return further;
}

/**********************************************************************************************************************/

void draw_chart(void)
{
    int i,j;
    int* x;
    int step=1;
    int compare_further=1;
    while(compare_further)
    {
        printf("in the step %d:",step);
        for(i=0;i<chart_size;i++)
        {
            printf("\n");
            for(j=0;j<no_variable;j++)
            {   if(chart[i][j]!=-1)
                    printf("%d ",chart[i][j]);
                else
                    printf("- ");
            }
            x=decode(i,chart);
            printf("(");
            for(j=1;j<x[0];j++)
            {
                printf("%d",x[j]);
                if(j!=x[0] -1)
                    printf(", ");
            }
            printf(")");
        }
        printf("\n");
        compare_further=compare();
        step++;
        printf("\n");
    }
    free(x);
}

/**********************************************************************************************************************/

int* decode(int position,int** source)
{
    int i,j,k;
    int x,n;
    int no_dash=0;
    int* combination;                                  //holding the combinations
    int no_combination;                                //no of combination possible
    int* source_holder;                                //temporary holder of the source

    for(j=0;j<no_variable;j++)                          //counting no of dashes
        if(source[position][j] == -1)
            no_dash++;

    no_combination=pow(2,no_dash);                       //no of combination is 2^no_dashes
    combination = malloc((no_combination*no_dash)*sizeof(int));

    for(i=(no_combination*no_dash-1) , x=(no_combination-1) ; i>=0 ; i-=no_dash , x--)         //creating a 1D array containing the combinations
    {
        n=x;
        for(j=i+no_dash;j>i;j--)
        {
            combination[j-no_dash]=n%2;
            n/=2;
        }
    }

    decoded=malloc((no_combination+1)*sizeof(int));                         //holding decoded value
    decoded[0]=no_combination+1;
    source_holder=malloc(no_variable*sizeof(int));                          //creating a source holder

    for(i=0 , k=0;i<no_combination;i++)                                     //putting the combinations and saves the decimal value in decoded
    {
        for(j=0;j<no_variable;j++)
            source_holder[j]=source[position][j];

        for(j=0;j<no_variable;j++)
            if(source_holder[j] == -1)
            {
                source_holder[j] = combination[k++];
            }
        x=0;
        for(j=no_variable-1;j>=0;j--)
        {
            n=no_variable-j-1;
            x = x+ (source_holder[j]*pow(2,n));
        }
        decoded[i+1]=x;
    }

    free(source_holder);                              //don't need it further
    free(combination);                                  //don't need it further

    return decoded;                                     //returning the decodded values
}

/**********************************************************************************************************************/

void find_PI(int position)                              //creating a  PI (holds all PI)
{
    int i,j;
    int combination=(no_m_term*(no_m_term-1))/2;
    if(combination==0)
        combination=1;
    int** temp_chart;
    temp_chart=malloc(combination*sizeof(int *));
    for(i=0;i<combination;i++)
    {
        temp_chart[i]=malloc(no_variable*sizeof(int));
    }
    if(no_PI==0)
    {
        for(j=0;j<no_variable;j++)
        {
        temp_chart[no_PI][j]=chart[position][j];
        }
        no_PI++;
    }
    else
    {
        for(i=0;i<no_PI;i++)
        {
            for(j=0;j<no_variable;j++)
            {
                temp_chart[i][j]=PI[i][j];
            }
        }
        for(j=0;j<no_variable;j++)
        {
            temp_chart[no_PI][j]=chart[position][j];
        }
        no_PI++;
    }

    PI=malloc(no_PI*sizeof(int *));
    for(i=0;i<no_PI;i++)
        PI[i]=malloc(no_variable*sizeof(int));

    for(i=0;i<no_PI;i++)
        for(j=0;j<no_variable;j++)
        {
            PI[i][j]=temp_chart[i][j];
        }
    free(temp_chart);
}

/**********************************************************************************************************************/

void print_PI(void)
{
    int i,j;
    int* x;
    printf("prime implicants are:\n");
    for(i=0;i<no_PI;i++)
    {
        for(j=0;j<no_variable;j++)
        {   if(PI[i][j]!=-1)
                printf("%d ",PI[i][j]);
            else
                printf("- ");
        }
        x=decode(i,PI);
        printf("(");
        for(j=1;j<x[0];j++)
        {
            printf("%d",x[j]);
            if(j!=x[0]-1)
                printf(" ,");
        }
        printf(")");
        printf("\n");
    }
    printf("\n");
    free(x);
}

/**********************************************************************************************************************/

void create_PI_table(void)
{
    int i,j,k;
    int no_cross;
    int no_row=no_PI;
    int no_column=no_m_term;
    int* x;

    covered=malloc(no_column*sizeof(int));                 //holds data about a mean-term is covered or not
    for(i=0;i<no_column;i++)
        covered[i]=0;
    non_essential=malloc(no_row*sizeof(int));
    essential=malloc(no_row*sizeof(int));                  //holds data about a PI is essential or not
    for(i=0;i<no_row;i++)
        essential[i]=non_essential[i]=0;

    PI_table=malloc(no_row*sizeof(int *));                      //creating a PI table
    for(i=0;i<no_row;i++)
        PI_table[i]=malloc(no_column*sizeof(int));
    for(i=0;i<no_row;i++)
        for(j=0;j<no_column;j++)
            PI_table[i][j]=0;

    for(i=0;i<no_row;i++)
    {
        x=decode(i,PI);
        for(j=1;j<x[0];j++)
        {
            for(k=0;k<no_m_term;k++)
            {
                if(m_term[k]==x[j])
                    PI_table[i][k]=1;
            }
        }
    }
    for(j=0;j<no_column;j++)
    {
        no_cross=0;
        for(i=0;i<no_row;i++)
        {
            if(PI_table[i][j]==1)
                no_cross++;
        }
        if(no_cross==1)
        {
            for(i=0;i<no_row;i++)
            {
                if(PI_table[i][j]==1)
                {
                    essential[i]=1;
                    goto next;
                }
            }
            next:
            for(k=0;k<no_column;k++)
            {
                if(PI_table[i][k]==1)
                {
                    covered[k]=1;
                }
            }
        }
    }
    k=0;
    for(i=0;i<no_m_term;i++)
        if(covered[i]==1)
            k++;
    if(k==no_m_term)
        allcovered=1;
    else
        allcovered=0;
}

/**********************************************************************************************************************/

void draw_PI_table(void)
{
    int i,j;
    int* x;
    int no_row=no_PI;
    int no_column=no_m_term;
    printf("PI table is:\n");                     //printing the PI table
    for(i=0;i<no_column;i++)
        printf("--------");
    printf("\n");
    for(i=0;i<no_m_term;i++)
    {
        printf("%d\t",m_term[i]);
        if(i==no_m_term-1)
            printf("\n");
    }
    for(i=0;i<no_column;i++)
        printf("--------");
    for(i=0;i<no_row;i++)
    {
        printf("\n");
        for(j=0;j<no_column;j++)
        {
            if(PI_table[i][j]==1)
                printf("x\t");
            else
                printf(" \t");
        }
        printf("  |  ");
        for(j=0;j<no_variable;j++)
        {   if(PI[i][j]!=-1)
                printf("%d ",PI[i][j]);
            else
                printf("- ");
        }
        x=decode(i,PI);
        printf("(");
        for(j=1;j<x[0];j++)
        {
            printf("%d",x[j]);
            if(j!=x[0]-1)
                printf(" ,");
        }
        printf(")\n");
    }
    for(i=0;i<no_column;i++)
        printf("--------");                                                         //end of printing PI table
    printf("\n\n");
    free(x);
}

/**********************************************************************************************************************/

void print_essential_PI(void)
{
    int i,j;
    int no_essential=0;
    int* x;
    printf("essential prime implicants are :\n");
    for(i=0;i<no_PI;i++)
    {
        if(essential[i]==1)
        {
            for(j=0;j<no_variable;j++)
            {
                if(PI[i][j]!=-1)
                    printf("%d ",PI[i][j]);
                else
                    printf("- ");
            }
            x=decode(i,PI);
            printf("(");
            for(j=1;j<x[0];j++)
            {
                printf("%d",x[j]);
                if(j!=x[0]-1)
                printf(" ,");
            }
            printf(")");
            no_essential++;
            printf("\n");
        }
    }
    if(no_essential==0)
        printf("no essential PI\n");
    printf("\n");
}

/**********************************************************************************************************************/

void create_reduced_PI_table(void)
{
    int i,j,k;
    no_r_PI=0;
    no_r_m_term=0;
    int* temp_r_PI_table;
    int capacity;
    for(i=0;i<no_m_term;i++)
    {
        if(covered[i]!=1)
            no_r_m_term++;
    }
    for(i=0;i<no_PI;i++)
    {
        if(essential[i]!=1 && non_essential[i]!=1)
            no_r_PI++;
    }
    capacity=no_r_m_term*no_r_PI;
    temp_r_PI_table=malloc(capacity*sizeof(int));

    reduced_PI_table=malloc(no_r_PI*sizeof(int*));
    for(i=0;i<no_r_PI;i++)
        reduced_PI_table[i]=malloc(no_r_m_term*sizeof(int));

    for(i=0,k=0;i<no_PI;i++)
    {
        if(essential[i]!=1 && non_essential[i]!=1)
        {
            for(j=0;j<no_m_term;j++)
            {
                if(covered[j]!=1)
                {
                    temp_r_PI_table[k++]=PI_table[i][j];
                }
            }
        }

    }
    for(i=0,k=0;i<no_r_PI;i++)
    {
        for(j=0;j<no_r_m_term;j++)
        {
            reduced_PI_table[i][j]=temp_r_PI_table[k++];
        }
    }

    free(temp_r_PI_table);
}

/**********************************************************************************************************************/

void draw_reduced_PI_table(void)
{
    int i,j;
    int* x;

    printf("reduced PI table is:\n");
    for(i=0;i<no_r_m_term;i++)
        printf("--------");
    printf("\n");
    for(i=0;i<no_m_term;i++)
    {
        if(covered[i]!=1)
            printf("%d\t",m_term[i]);
        if(i==no_m_term-1)
            printf("\n");
    }
    for(i=0;i<no_r_m_term;i++)
        printf("--------");
    for(i=0;i<no_PI;i++)
    {
        if(essential[i]!=1 && non_essential[i]!=1)
        {
            printf("\n");
            for(j=0;j<no_m_term;j++)
            {
                if(covered[j]!=1)
                {
                    if(PI_table[i][j]==1)
                        printf("x\t");
                    else
                        printf(" \t");
                }
            }
            printf("  |  ");
            for(j=0;j<no_variable;j++)
            {
                if(PI[i][j]!=-1)
                    printf("%d ",PI[i][j]);
                else
                    printf("- ");
            }
            x=decode(i,PI);
            printf("(");
            for(j=1;j<x[0];j++)
            {
                printf("%d",x[j]);
                if(j!=x[0]-1)
                    printf(" ,");
            }
            printf(")\n");
        }

    }
    for(i=0;i<no_r_m_term;i++)
        printf("--------");
    printf("\n\n");
}

/**********************************************************************************************************************/

void find_non_essential(void)
{
    int i,j;
    int no_cross;
    int max_no_cross=0;
    int index=0;
    int* temp_non_essential;
    int* temp_covered;

    create_reduced_PI_table();
    if(reduce==0)
    {
        draw_reduced_PI_table();
        reduce++;
    }
    temp_covered=malloc(no_r_m_term*sizeof(int));
    for(i=0;i<no_r_m_term;i++)
        temp_covered[i]=0;

    temp_non_essential=malloc(no_r_PI*sizeof(int));
    for(i=0;i<no_r_PI;i++)
        temp_non_essential[i]=0;

    for(i=0;i<no_r_PI;i++)
    {
        no_cross=0;
        for(j=0;j<no_r_m_term;j++)
        {
            if(reduced_PI_table[i][j]==1)
                no_cross++;
        }
        if(no_cross>max_no_cross)
        {
            max_no_cross=no_cross;
            index=i;
        }
    }
    temp_non_essential[index]=1;

    for(j=0;j<no_r_m_term;j++)
    {
        if(reduced_PI_table[index][j]==1)
            temp_covered[j]=1;
    }

    for(i=0,j=0;i<no_PI;i++)
    {
        if(essential[i]!=1 && non_essential[i]!=1)
            non_essential[i]=temp_non_essential[j++];
    }

    for(i=0,j=0;i<no_m_term;i++)
    {
        if(covered[i]!=1)
            covered[i]=temp_covered[j++];
    }

    for(i=0,j=0;i<no_m_term;i++)
        if(covered[i]==1)
            j++;

    free(temp_non_essential);
    free(temp_covered);
    free(reduced_PI_table);

    if(j==no_m_term)
        allcovered=1;
    else
    {
        allcovered=0;
        find_non_essential();
    }
}

/**********************************************************************************************************************/

void print_non_essential(void)
{
    int i,j;
    int no_non_essential=0;
    int* x;
    printf("non essential prime implicants are :\n");
    for(i=0;i<no_PI;i++)
    {
        if(non_essential[i]==1)
        {
            for(j=0;j<no_variable;j++)
            {
                if(PI[i][j]!=-1)
                    printf("%d ",PI[i][j]);
                else
                    printf("- ");
            }
            x=decode(i,PI);
            printf("(");
            for(j=1;j<x[0];j++)
            {
                printf("%d",x[j]);
                if(j!=x[0]-1)
                printf(" ,");
            }
            printf(")");
            no_non_essential++;
            printf("\n");
        }
    }
    if(no_non_essential==0)
        printf("no non essential PI\n");
    printf("\n");
}

/**********************************************************************************************************************/

void solution(void)
{
    int i,j=0;
    int term;
    int no_essential=0;
    int no_non_essential=0;
    for(i=0;i<no_PI;i++)
    {
        if(essential[i]==1)
            no_essential++;
    }
    for(i=0;i<no_PI;i++)
    {
        if(non_essential[i]==1)
            no_non_essential++;
    }
    term=no_essential+no_non_essential;

    printf("minimized function is :\n");
    if(no_essential!=0)
    for(i=0;i<no_PI;i++)
    {
        if(essential[i]==1)
        {
            cover(i,PI);
            j++;
            if(j!=term)
                printf(" + ");
        }
    }
    if(no_non_essential!=0)
    for(i=0;i<no_PI;i++)
    {
        if(non_essential[i]==1)
        {
            cover(i,PI);
            j++;
            if(j!=term)
                printf(" + ");
        }
    }

    printf("\n");
}

/**********************************************************************************************************************/

void cover(int index,int** source)
{
    char i='A';
    int j;

    for(j=0;j<no_variable;j++)
    {
        if(source[index][j]!=-1)
        {
            printf("%c",i);
        }
        if(source[index][j]==0)
        {
            printf("'");
        }
        i++;
    }
}

/**********************************************************************************************************************/

void rand_no()
{
    int i,n;
    int p;
    int k;
    time_t t;
    n=pow(2,no_variable);
    random_no=malloc(n*sizeof(int));
    for(i=0;i<n;i++)
        random_no[i]=i;
    srand((unsigned)time(&t));
    for(i=0;i<n;i++)
    {
        p=rand()%n;
        k=random_no[p];
        random_no[p]=random_no[i];
        random_no[i]=k;
    }
}

/**********************************************************************************************************************/
