#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
//#include <omp.h>
#include <math.h>
#include <algorithm>
#include <iomanip>
//#include <experimental/filesystem>

#define N_THREAD 8

using namespace std;
void run_DTW_Generation(string dataPath, string savePath, string Subject, string Reference);
void readCSV(FILE *fp, std::vector<std::string>& vls,int count);
void CSV2Memory(string filename, float* &output_data, int &TotalSize);
float dtw_c(float *s, float *t, int ns, int nt);
void MinAndMaxNorm(float *input, int inputSize, float * &output);


int write_array(float* &out_data, float * Data,int itr){
	   int sz=0;
	
	   while(1)
		{
			if((Data[itr]==-1) || (Data[itr]==0))
				break;
            out_data=(float*)realloc(out_data, sizeof(float) * (sz+1));
            out_data[sz]=Data[itr];
      
            sz++;
            itr++;
   
        } 
        return itr;

}

void readCSV(FILE *fp, std::vector<std::string>& vls, int count)
{
	vls.clear();

	if (!fp)
		return;
	char buf[1000];

	if (!fgets(buf, 999, fp))
		return;
	std::string s = buf;

	int p, q;
	q = -1;
	// loop over columns

	while (1) {
		p = q;
		q = s.find_first_of(",\n", p + 1);
		if(q==-1){
				break;


		}

			vls.push_back(s.substr(p + 1, q - p - 1));


	}
}


void CSV2Memory(string filename, float* &output_data, int &TotalSize){

	FILE * Data_point = fopen(filename.c_str(), "r");

	if (!Data_point) {
		fclose(Data_point);
		return;
	}

	vector<string> vector_Data;

	int count =0;
	while (true){

		readCSV(Data_point, vector_Data, count++);

		if (vector_Data.size() == 0) break;

		output_data = (float*)realloc(output_data, sizeof(float) * (TotalSize + vector_Data.size()));
		for (int i = 0; i < vector_Data.size(); i++) {
			output_data[TotalSize + i] = atof(vector_Data[i].c_str());
			
		}
		TotalSize += vector_Data.size();
	}

	fclose(Data_point);
}

float dtw_c(float *s, float *t, int ns, int nt)
{
	float d = 0;
	float ** D;
	int i, j;
	int j1, j2;
	float cost, temp;
	// create D
	D = (float **)malloc((ns + 1)*sizeof(float *));
	for (i = 0; i<ns + 1; i++)
	{
		D[i] = (float *)malloc((nt + 1)*sizeof(float));
	}

	// initialization
	for (i = 0; i<ns + 1; i++)
	{
		for (j = 0; j<nt + 1; j++)
		{
			D[i][j] = -1;
		}
	}
	D[0][0] = 0;

	// dynamic programming
	for (i = 1; i <= ns; i++)
	{

		j1 = 1;
		j2 = nt;

		for (j = j1; j <= j2; j++)
		{
			cost = sqrtf((s[i - 1] - t[j - 1])*(s[i - 1] - t[j - 1]));

			temp = D[i - 1][j];
			if (D[i][j - 1] != -1)
			{
				if (temp == -1 || D[i][j - 1]<temp) temp = D[i][j - 1];
			}
			if (D[i - 1][j - 1] != -1)
			{
				if (temp == -1 || D[i - 1][j - 1]<temp) temp = D[i - 1][j - 1];
			}

			D[i][j] = cost + temp;
		}
	}


	d = D[ns][nt];


/*	for(i=0;i<ns+1;i++)
	{
	for(j=0;j<nt+1;j++)
	{
	printf("%f  ",D[i][j]);
	}
	printf("\n");
	}
	*/

	// free D
	for (i = 0; i<ns + 1; i++)
	{
		free(D[i]);
	}
	free(D);

	return d;
}

void MinAndMaxNorm(float *input,int ind, int inputSize, float * &output){
	float max_value = *max_element(input+ind, input + (ind+inputSize));
	float min_value = *min_element(input+ind, input + (ind+inputSize));
	int j=0;
	for (int i = ind; i < inputSize; i++){
		output[j] = (input[i] - min_value) / (max_value - min_value);
		j++;}

}

void run_DTW_Generation(string mode,string users[2],string dataPath, string labelpath, string savePath, string Subject, string Reference){
	
	float* Subject_Data = NULL;
	int Subject_N_Data = 0;
	float* Subject_Label = NULL;
	int Subject_N_Label = 0;
	

	CSV2Memory(dataPath+"DOG_"+users[0]+".csv", Subject_Data, Subject_N_Data);

	CSV2Memory(labelpath + "Label_"+users[0]+".csv", Subject_Label, Subject_N_Label);


	float* Reference_Data = NULL;
	int Reference_N_Data = 0;
	float* Reference_Label = NULL;
	int Reference_N_Label = 0;

	CSV2Memory(dataPath + "DOG_"+users[1]+".csv", Reference_Data, Reference_N_Data);

	CSV2Memory(labelpath +"Label_"+users[1]+".csv", Reference_Label, Reference_N_Label);


    int ref_eat=0;
    int ref_size=0;

    if(mode.compare("test")==0){
        for(int i=0; i<Reference_N_Label;i++){
        if(Reference_Label[i]==1)
            ref_eat++;
        }
        ref_size=ref_eat;

    }
    else
        ref_size=Reference_N_Label;

	 float **DTW_feature = (float**)malloc(sizeof(float*) * Subject_N_Label*ref_size);

	for (int i = 0; i < Subject_N_Label*ref_size; i++)
		DTW_feature[i] = (float*)malloc(sizeof(float) * 486);


	int iSubject = 0;
	int iReference = 0;
	int s1=0,s2;
	int test_ref=0;


	int subject_idx=0, reference_idx=0, i, j, iCompare,indx=-1;
//	omp_set_num_threads(Reference_N_Label);

//#pragma omp parallel for private(iCompare)
	for (iCompare = 0; iCompare < Subject_N_Label*Reference_N_Label; iCompare++){
		int i = iCompare / Reference_N_Label;
		int j = iCompare % Reference_N_Label;

		int flag=0;
		if(j==0){
			iSubject=s1;
			s2=0;

		    test_ref=0;

			if((Reference_Label[test_ref]==0) && mode.compare("test")==0)
			{
			    flag=1;}
			else
			    indx++;
			test_ref++;

		}
		else{
			s1=iSubject;

			if((Reference_Label[test_ref]==0) && mode.compare("test")==0)
			    flag=1;
			else
			    indx++;
			test_ref++;
		}

		cout<<"itr";
		cout<<"\n";
		cout<<iCompare;
		cout<<"\n";

		for (int iSensor = 0; iSensor < 18; iSensor++){


			for (int iOctave = 0; iOctave < 3; iOctave++){
				for (int iScale = 0; iScale < 9; iScale++){
			

					float* subject_one=NULL;
					float* reference_one=NULL;

					subject_idx=s1;
				
			
					s1=write_array(subject_one,Subject_Data,subject_idx)+1;
	                
                   	reference_idx=s2;

                   	s2=write_array(reference_one,Reference_Data,reference_idx)+1;

                   	if(flag!=1){

                 	float* norm_subject_one=(float *)malloc((s1-subject_idx-1)*sizeof(float));
                 	float* norm_reference_one=(float *)malloc((s2-reference_idx-1)*sizeof(float));


				//	MinAndMaxNorm(subject_one, subject_idx,s1-subject_idx-1, norm_subject_one);
				
				//	MinAndMaxNorm(reference_one, reference_idx,s2-reference_idx-1, norm_reference_one);

		DTW_feature[indx][iSensor * 27 + iOctave * 9 + iScale] = dtw_c(subject_one, reference_one, s1-subject_idx-1, s2-reference_idx-1);
		}
				}

			}
		} 

	} 
    string save_fileanme;
    if(mode.compare("train")==0)
	    save_fileanme = savePath+"train.txt";
	else
	    save_fileanme = savePath+"test.txt";
	const char* DTW_filename = save_fileanme.c_str();
	ofstream Out_File;
	Out_File.open(DTW_filename,std::fstream::app);
	int ref_ind=0;
	for (int m = 0; m < Subject_N_Label*ref_size; m++){
	    Out_File << fixed << setprecision(5) << users[0];
	    Out_File << ",";

	    int j = m % Reference_N_Label;

		for (int n = 0; n < 486; n++){
		     cout<<n;
		    cout<<" ";
		    cout<<DTW_feature[m][n];
		    cout<<"\n";
			Out_File << fixed << setprecision(5) << DTW_feature[m][n];
			if(n!=485)
			    Out_File << ",";


		}
		if(mode.compare("test")!=0){
		Out_File << ",";
		 if (j==0)
            ref_ind=0;
         else
            ref_ind++;
        if(Reference_Label[ref_ind]==1)
            Out_File << "T";
        else
            Out_File << "F";
        }
		Out_File << endl;
	}
	Out_File.close();

}



int main(){

	string dataPath = "DOG/";
	string labelpath = "data/";
	string savePath = "DTW/";
	string Subject = "DOG/";
	string Reference = "DOG/";
	string train[2]={"3","1"};
	string test[1]={"2"};
	for(int i=0;i<sizeof(train);i++){
	   for(int j=0;j<sizeof(train);j++){
	   string users[2];

	   if(i!=j){
	   users[0]=train[i];
	   users[1]=train[j];
	   cout<<users[0];
	   cout<<users[1];
        run_DTW_Generation("train",users,dataPath, labelpath, savePath, Subject, Reference);
        }
        }
    }
    for(int i=0;i<sizeof(test);i++){
        for(int j=0;j<sizeof(train);j++){
        string users[2];
	    users[0]=test[i];
	    users[1]=train[j];

        run_DTW_Generation("test",users,dataPath, labelpath, savePath, Subject, Reference);
    }
    }
	return 0;
}
