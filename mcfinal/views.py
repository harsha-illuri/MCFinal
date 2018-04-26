from django.shortcuts import render
from mcfinal import segments
import neural
# Create your views here.
import time
from django.http import HttpResponse, HttpRequest
def test(request):
    data = request.GET.get('data', '')
    if len(data) == 0:
        HttpResponse(404)
    print(data)
    data = data.split('~')[1:]
    print(data)
    train = list(data[0])
    test = list(data[1])
    print(train)
    print(test)
    # segments.py
    segments.main(train, test)

    # call cpp file with subprocess

    neural.main(train, test)

    #call neural.py

    return HttpResponse(data)