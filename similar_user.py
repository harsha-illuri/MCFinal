import csv


def similar_user(predictions, train, test):
    train = ['1', '3']
    test = ['2']
    f1 = open('data/' + 'Label_' + test[0] + '.csv', 'r')
    data = csv.reader(f1)
    test_segs=sum(1 for row in data)
    Num_Eat = {}

    for x in train:
        f = open('data/' + 'Label_' + x + '.csv', 'r')
        data = csv.reader(f)
        count = 0
        for row in data:
            if row[0] == '1':
                count = count + 1
        Num_Eat[x] = count

    print(Num_Eat)

    Keys = Num_Eat.keys()
    pred_itr = 0
    similar_segments = {}

    for x in Keys:
        seg_list=[]
        for seg in range(0,test_segs):
            similar_seg = 0
            for i in range(0, Num_Eat[x]):
                print(predictions[pred_itr][0])
                if predictions[pred_itr][0] == 1:
                    similar_seg = similar_seg + 1
                pred_itr=pred_itr+1
            print(similar_seg)
            seg_list.append(similar_seg)
        similar_segments[x]=seg_list
    print(similar_segments)

    seg_w=list(similar_segments.values())
    print(seg_w)

    test_segment_weight=[]

    for x in range(0,len(seg_w)):
        if x==0:
            test_segment_weight=seg_w[0]
        else:
            for y in range(0,len(seg_w[x])):
                test_segment_weight[y]=test_segment_weight[y]+seg_w[x][y]

    print(test_segment_weight)
    Threshold=test_segs*0.7
    predict=[]
    for x in test_segment_weight:
        if x >= Threshold:
            predict.append('Eat')
        else:
            predict.append('Other')

    print(predict)



