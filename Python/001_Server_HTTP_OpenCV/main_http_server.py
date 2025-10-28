import tkinter as tk
import urllib.request as req
from PIL import Image
from PIL import ImageTk
import os
import cv2
import time

images_folder = r"C:\000_Dane\090_Python\010_SerwerHTTP\images"
file_path = r"C:\000_Dane\090_Python\010_SerwerHTTP\images\temp_image.jpg"
cam_url = "http://192.168.9.200:1080/snapshot.cgi?loginuse=admin&loginpas="
snapshot_url = "http://192.168.9.200:1080/snapshot.cgi?loginuse=admin&loginpas="

def get_picture(gp_cam_url=None, gp_file_path=None, gp_display_picture=False):
    response = req.urlopen(url=gp_cam_url)
    cam_reply = req.urlretrieve(url=gp_cam_url, filename=gp_file_path)
    if gp_display_picture is True:
        os.system(gp_file_path)
    return response, cam_reply


def main(response):
    try:
        # Relative Path
        img = Image.open(response)
        img.format = "JPG"

        def button_click_exit_mainloop(event):
            event.widget.quit()  # this will cause mainloop to unblock.

        root = tk.Tk()
        root.bind("<Button>", button_click_exit_mainloop)
        root.geometry('+%d+%d' % (100, 100))
        root.geometry('%dx%d' % (img.size[0], img.size[1]))
        tkpi = ImageTk.PhotoImage(img)
        label_image = tk.Label(root, image=tkpi)
        label_image.place(x=0, y=0, width=img.size[0], height=img.size[1])
        root.title("Nazwa obrazka")
        root.mainloop()  # wait until user clicks the window

    except IOError:
        raise


def test_openCV(img=None):
    try:
        cv2.namedWindow('image', cv2.WINDOW_NORMAL)  # Image resizing
        cv2.imshow('image', img)
        cv2.waitKey()  # Wait for any key pressed
    finally:
        cv2.destroyAllWindows()


def frame_diff(frame_1=None, frame_2=None, frame_3=None):
    # Calculate diff
    diff_fr3_to_fr2 = cv2.absdiff(frame_3, frame_2)
    diff_fr2_to_fr1 = cv2.absdiff(frame_2, frame_1)
    # Return common part
    test_openCV(img=diff_fr2_to_fr1)
    test_openCV(img=diff_fr3_to_fr2)

    diff_and = cv2.bitwise_xor(diff_fr2_to_fr1, diff_fr3_to_fr2)

    test_openCV(img=diff_and)
    return diff_and


def motion_detection():
    fr1 = os.path.join(images_folder, "01.jpg")
    fr2 = os.path.join(images_folder, "02.jpg")
    fr3 = os.path.join(images_folder, "04.jpg")
    fr4 = os.path.join(images_folder, "04.jpg")

    fr1_color = cv2.imread(filename=fr1, flags=cv2.IMREAD_COLOR)
    fr1_grey = cv2.cvtColor(fr1_color, cv2.COLOR_RGB2GRAY)
    # test_openCV(img=fr1_grey)

    fr2_color = cv2.imread(filename=fr2, flags=cv2.IMREAD_COLOR)
    fr2_grey = cv2.cvtColor(fr2_color, cv2.COLOR_RGB2GRAY)
    # test_openCV(img=fr2_grey)

    fr3_color = cv2.imread(filename=fr3, flags=cv2.IMREAD_COLOR)
    fr3_grey = cv2.cvtColor(fr3_color, cv2.COLOR_RGB2GRAY)
    # test_openCV(img=fr3_grey)

    # Thresh hold
    thresh = cv2.threshold(frame_diff(fr1_grey, fr2_grey, fr3_grey), 70, 255, cv2.THRESH_BINARY)[1]
    # Thresh hold enhancement
    thresh = cv2.dilate(thresh, None, iterations=2)
    # Wykrywanie krawędzi
    (counts, _) = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    for c in counts:
        # Ignorowanie zbyt małych obszarów
        if cv2.contourArea(c) < 100:
            continue
        # Naniesienie ramki na obszar w którym wykryto zmianę
        # na ostatnio pobraną klatkę
        (x, y, w, h) = cv2.boundingRect(c)
        cv2.rectangle(fr3_grey, (x, y), (x + w, y + h), (0, 255, 0), 2)

    # Wyświetlenie klatki z naniesionymi zaznaczeniami
    test_openCV(img=fr3_grey)


def get_frame(gf_snapshot_url=None, gf_file_path=""):
    try:
        req.urlretrieve(url=gf_snapshot_url, filename=gf_file_path)
    except:
        print('Problem z plikiem')
    pass


def mail_loop(ml_snapshot_url=None, ml_file_path=None):
    ml_execute = True
    ml_fr1 = None
    ml_fr2 = None
    ml_fr3 = None
    first_loop = True
    save_image = False

    cv2.namedWindow('Camera Image', cv2.WINDOW_NORMAL)  # Image resizing
    cv2.namedWindow('Motion', cv2.WINDOW_NORMAL)  # Image resizing

    # cam = cv2.VideoCapture(0)

    while ml_execute:
        get_frame(gf_snapshot_url=ml_snapshot_url, gf_file_path=ml_file_path)
        img_org = cv2.imread(filename=ml_file_path, flags=cv2.IMREAD_COLOR)

        # img_org = cam.read()[1]

        cv2.imshow('Camera Image', img_org)
        img = cv2.cvtColor(img_org, cv2.COLOR_RGB2GRAY)

        # First loop
        if first_loop:
            first_loop = False
            ml_fr1 = img
            ml_fr2 = img
            ml_fr3 = img

        # Read new frame
        ml_fr1 = ml_fr2
        ml_fr2 = ml_fr3
        ml_fr3 = img

        # Calculate diff
        diff_fr3_to_fr2 = cv2.absdiff(ml_fr3, ml_fr2)
        diff_fr2_to_fr1 = cv2.absdiff(ml_fr2, ml_fr1)

        ml_diff = cv2.bitwise_and(diff_fr2_to_fr1, diff_fr3_to_fr2)

        # Thresh hold
        thresh = cv2.threshold(ml_diff, 70, 255, cv2.THRESH_BINARY)[1]
        # Thresh hold enhancement
        thresh = cv2.dilate(thresh, None, iterations=2)
        # Wykrywanie krawędzi
        (counts, _) = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        for c in counts:
            # Ignorowanie zbyt małych obszarów
            if cv2.contourArea(c) < 100:
                continue
            # Naniesienie ramki na obszar w którym wykryto zmianę
            # na ostatnio pobraną klatkę
            (x, y, w, h) = cv2.boundingRect(c)
            cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
            save_image = True

        # Save image
        if save_image:
            save_image = False
            first_loop = True
            cv2.imwrite(os.path.join(images_folder, "{}.jpg".format(time.time())), img_org)

        cv2.imshow('Motion', img)

        my_key = cv2.waitKey(1000)
        if my_key > 0:
            ml_execute = False
            cv2.destroyAllWindows()
        print('New frame: {}'.format(time.perf_counter()))
    pass




if __name__ == "__main__":
    # resp, cam_rpl = get_picture(gp_cam_url=cam_url, gp_file_path=file_path)
    # main(response=resp)
    # print(cam_rpl)
    # test_openCV(file_path)
    # motion_detection()
    mail_loop(ml_snapshot_url=snapshot_url, ml_file_path=file_path)
