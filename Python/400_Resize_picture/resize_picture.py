import os
from glob import glob
from PIL import Image


def createListOfFiles(dirPaths=[os.curdir]):
    listOfFiles = []
    for dirPath in dirPaths:
        pathName = os.path.join('', dirPath, '**', '*.jpg')
        listOfFiles.extend(glob(pathname=pathName, recursive=True))

    return listOfFiles


def get_img_attributes(img_path=''):
    img = Image.open(img_path)
    return img.size


def calculate_w_h(img_path=''):
    img_width, img_height = get_img_attributes(img_path)
    k5_width = 1024

    if img_width >= img_height:
        if (img_width > k5_width):
            resize_ratio = k5_width / img_width
        else:
            resize_ratio = 1.0
    else:
        if (img_height > k5_width):
            resize_ratio = k5_width / img_height
        else:
            resize_ratio = 1.0

    cal_width = int(img_width * resize_ratio)
    cal_height = int(img_height * resize_ratio)
    return cal_width, cal_height


def create_dir(dir_path=''):
    components = dir_path.split('\\')
    full_dir_path = ''
    for component in components:
        full_dir_path = os.path.join(full_dir_path,
                                     component.replace(':', ':\\'))
        if not os.path.exists(full_dir_path):
            os.mkdir(full_dir_path)


def resize_img(img_path='', img_save_path='', img_width=100, img_height=100):
    img = Image.open(img_path)
    new_img = img.resize((img_width, img_height), Image.ANTIALIAS)
    save_dir = os.path.dirname(img_save_path)
    print(img_save_path)
    if not os.path.exists(save_dir):
        create_dir(dir_path=save_dir)
    new_img.save(img_save_path, "JPEG", optimize=True)


def resize_image(input_base_path='', img_path='', save_base_path=''):
    img_full_path = os.path.join(input_base_path, img_path)
    img_save_path = os.path.join(save_base_path, img_path)
    cal_width, cal_height = calculate_w_h(img_path=img_full_path)
    resize_img(img_full_path, img_save_path,
               img_width=cal_width, img_height=cal_height)


if __name__ == '__main__':

    print('START')
    print('Reading list of files')
    base_path = r'F:\Zdjecia'
    myFiles = createListOfFiles([base_path])
    new_base = r'F:\Male_zdjecia'
    for img_file in myFiles:
        img_short_path = img_file.replace('{}\\'.format(base_path), '')
        resize_image(input_base_path=base_path,
                     img_path=img_short_path,
                     save_base_path=new_base)
