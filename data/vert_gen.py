import re

def vert_gen(input_path, output_path):
    vertices = []
    faces = []

    file = open(input_path, 'r')
    lines = file.readlines()
    for line in lines:
        if (line[0] == 'v'):
            vertices.append( re.findall(r"[-+]?\d*\.*\d+", line.strip()) )
        if (line[0] == 'f'):
            faces.append( re.findall(r'\d+', line.strip()) )
    # print(vertices)
    # print(faces)
    file.close()

    file = open(output_path, 'w')

    for face in faces:
        line = ""
        for index in face:
            i = int(index)
            # print(i - 1)
            for coordinate in vertices[i - 1]:
                line += str(coordinate) + ", "
            line += "\n"
        file.write(line)
        print(line)

    # print(faces)
    # print(vertices)
    


vert_gen("/home/krystian-jasionek/Studia/Semestr V/PGK/opengl-lighting/data/cube.obj", "cube.txt")
