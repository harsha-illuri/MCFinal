import numpy as np
from numpy import *
import math, scipy
import csv


def downsample(a,R):
    pad_size = math.ceil(float(a.size) / R) * R - a.size
    a_padded = np.append(a, np.zeros(pad_size) * np.NaN)
    return scipy.nanmean(a_padded.reshape(-1, R), axis=1)

def gaussian(data_size, data, sig):
    x=np.linspace(-data_size/2, data_size/2, data_size)
    Gaussian= np.exp(-np.power(x , 2.) / (2 * np.power(sig, 2.)))
    data = list(map(float, data))
    #Gaussian = list(map(float, Gaussian))
    return  np.convolve(data, Gaussian, mode="same")

def smoothen_data(data):
    DoG_data =[]
    space_scale_data = []

    for i in range(1,4):

        scale_step = [1,np.power(2,1/4),np.power(2,1/2),np.power(2,3/4),np.power(2,2)]
        for j in range(0,5):
            if i==1:
                ocatave_data = data

            else :
               # ocatave_data = downsample(space_scale_data[5*i-6],2)
                ocatave_data = space_scale_data[5*i-6][::2]
            space_scale_data.append(gaussian(len(ocatave_data),ocatave_data,scale_step[j]))
          #  print(space_scale_data[-1])
            space_scale_data[-1]=np.append(space_scale_data[-1],-1)
           # print(space_scale_data[-1])

            if(j!=0):{
                DoG_data.append((space_scale_data[len(space_scale_data)-1]) - (space_scale_data[len(space_scale_data)-2]))
               # print(DoG_data[0])
               # DoG_data[-1]=np.append(DoG_data[-1],-1)

            }
    return space_scale_data,DoG_data

def main(train, test):

        user = train+test
        for x in user:
            f = open('data/'+'Data_'+x+'.csv','r')

            data = csv.reader(f)
            for row in data:
                   # print(row
                space_scale_data,DoG_data=smoothen_data(row)
                f1 = open("DOG/"+"DOG_"+x+".csv", "a")
                writer = csv.writer(f1)
                writer.writerows(space_scale_data)
                writer.writerows(DoG_data)
            f.close()
            

main()















