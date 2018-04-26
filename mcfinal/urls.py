from django import urls

from django.contrib import admin
from django.urls import path, include

from mcfinal import views
urlpatterns = [
    path('$', views.test, name = 'test'),
]
