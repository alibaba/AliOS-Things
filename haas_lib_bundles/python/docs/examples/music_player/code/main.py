# -*- encoding: utf-8 -*-
from music import MusicPlayer

player = MusicPlayer()
player.createPage()
print('Music Player started')

my_music = {
    "title":"spring_short",
    "album":"Darius",
    "album_url": '',
    "url":"file://data/pyamp/spring.mp3",
    "duration":30,
    "favorite": False
}

player.addToList(my_music)
print('add new music into play list')
