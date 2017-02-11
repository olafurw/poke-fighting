from PIL import Image
import os

colors = [
	(34,128,0), (105,140,138), (45,80,89), 
	(19,50,77), (35,63,140), (177,163,217), 
	(87,26,102), (255,128,213), (153,77,107), 
	(229,115,115), (204,143,102), (204,153,51), 
	(121,128,32), (27,51,0), (26,102,66), 
	(191,251,255), (0,102,153), (153,180,204)
]

for filename in os.listdir('data'):
	lines = [line.rstrip('\n') for line in open('data/' + filename)]
	
	colorList = []
	
	for line in lines:
		for character in line:
			intValue = ord(character) - 65
			
			if intValue >= 0:
				colorList.append(colors[intValue])
	
	im = Image.new('RGB', (256, 256))
	im.putdata(colorList)
	im.save('images/' + filename + '.png')