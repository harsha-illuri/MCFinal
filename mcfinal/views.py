from django.shortcuts import render

# Create your views here.
import time
from django.http import HttpResponse, HttpRequest
def test(request):
    data = request.GET.get('data', '')
    if len(data) == 0:
        HttpResponse(404)
    print(data)
    return HttpResponse(data)