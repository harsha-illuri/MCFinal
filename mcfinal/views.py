from django.shortcuts import render

# Create your views here.
import time
from django.http import HttpResponse, HttpRequest
def test(request):
    time.sleep(10)
    return HttpResponse('hello')