import pandas as pd

from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout
from keras.models import model_from_json

import similar_user
def main(inp_train, inp_test):
    train = pd.read_csv('DTW/train.txt', sep=",", header=None)
    test = pd.read_csv('DTW/test.txt', sep=",", header=None)
    # df = pd.read_csv("full.csv")
    train.columns = ["user_id"]+['node_' + str(i) for i in range(486)] + ["output"]
    test.columns = ["user_id"]+['node_' + str(i) for i in range(486)]

    for i, row in train.iterrows():

            if row["output"] == "F":
                row["output"] = 0
                # print("lol")
            else:
                row["output"] = 1
            # print(row["output"])
            train.loc[i, "output"] = row["output"]



    train.sample(frac=1)
    model = Sequential()
    model.add(Dense(486, activation='relu',input_dim=486))
    model.add(Dropout(0.3,noise_shape=None, seed=None))
    model.add(Dense(512, activation='relu'))
    model.add(Dropout(0.3,noise_shape=None, seed=None))
    model.add(Dense(256, activation='relu'))
    model.add(Dropout(0.3,noise_shape=None, seed=None))
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.3,noise_shape=None, seed=None))
    model.add(Dense(64, activation='relu'))
    model.add(Dropout(0.3,noise_shape=None, seed=None))
    #model.add(Dense(32, activation='tanh'))
    #model.add(Dropout(0.3,noise_shape=None, seed=None))
    #model.add(Dense(32, activation='tanh'))

    model.add(Dense(1, activation='sigmoid'))


    # Compile model
    model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

    # trainlength = (int)(train["node_0"].count() * 0.70)
    traindata = train
    testdata = test

    train_success_output = traindata.ix[:,487].values.tolist()
    train_features = traindata.ix[:, 1:487]

    # test_success_output = testdata.ix[:,487].values.tolist()
    test_features = testdata.ix[:,1:487]

    model.fit(train_features, train_success_output, epochs = 20, batch_size = 10)

    scores = model.evaluate(train_features, train_success_output)

    prediction = model.predict(test_features)
    # scores = model.evaluate(test_features, test_success_output)
    print(prediction)
    similar_user.similar_user(prediction, inp_train, inp_test )

    model_json = model.to_json()
    with open("model.json",'w') as json_file:
        json_file.write(model_json)
    model.save_weights("generallayers.h5")
    print("saved model to disk")
