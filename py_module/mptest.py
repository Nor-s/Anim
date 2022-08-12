# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
# from multiprocessing import freeze_support
import json
import cv2
import glm
import os 
import mediapipe as mp
import math
import matplotlib.pyplot as plt
from enum import auto, IntEnum
import copy
from PyQt5.QtWidgets import QApplication, QMainWindow,  QFileDialog
import sys
import argparse
from PyQt5 import QtCore,  QtWidgets

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(378, 387)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Dialog.sizePolicy().hasHeightForWidth())
        Dialog.setSizePolicy(sizePolicy)
        self.frame = QtWidgets.QFrame(Dialog)
        self.frame.setGeometry(QtCore.QRect(10, 10, 361, 341))
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.frame.sizePolicy().hasHeightForWidth())
        self.frame.setSizePolicy(sizePolicy)
        self.frame.setMinimumSize(QtCore.QSize(300, 300))
        self.frame.setObjectName("frame")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.tabWidget = QtWidgets.QTabWidget(self.frame)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)
        self.tabWidget.setMinimumSize(QtCore.QSize(300, 200))
        self.tabWidget.setContextMenuPolicy(QtCore.Qt.DefaultContextMenu)
        self.tabWidget.setTabPosition(QtWidgets.QTabWidget.North)
        self.tabWidget.setTabShape(QtWidgets.QTabWidget.Rounded)
        self.tabWidget.setUsesScrollButtons(False)
        self.tabWidget.setTabBarAutoHide(False)
        self.tabWidget.setObjectName("tabWidget")
        self.tab2 = QtWidgets.QWidget()
        self.tab2.setObjectName("tab2")
        self.layoutWidget = QtWidgets.QWidget(self.tab2)
        self.layoutWidget.setGeometry(QtCore.QRect(0, 10, 311, 221))
        self.layoutWidget.setObjectName("layoutWidget")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.layoutWidget)
        self.gridLayout_3.setContentsMargins(0, 0, 0, 0)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.lbl_display2 = QtWidgets.QLabel(self.layoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lbl_display2.sizePolicy().hasHeightForWidth())
        self.lbl_display2.setSizePolicy(sizePolicy)
        self.lbl_display2.setObjectName("lbl_display2")
        self.gridLayout_3.addWidget(self.lbl_display2, 2, 0, 1, 1)
        self.cmb_gif = QtWidgets.QComboBox(self.layoutWidget)
        self.cmb_gif.setObjectName("cmb_gif")
        self.gridLayout_3.addWidget(self.cmb_gif, 3, 1, 1, 1)
        self.lbl_display = QtWidgets.QLabel(self.layoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lbl_display.sizePolicy().hasHeightForWidth())
        self.lbl_display.setSizePolicy(sizePolicy)
        self.lbl_display.setObjectName("lbl_display")
        self.gridLayout_3.addWidget(self.lbl_display, 0, 0, 1, 1)
        self.btn_add_model = QtWidgets.QPushButton(self.layoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btn_add_model.sizePolicy().hasHeightForWidth())
        self.btn_add_model.setSizePolicy(sizePolicy)
        self.btn_add_model.setObjectName("btn_add_model")
        self.gridLayout_3.addWidget(self.btn_add_model, 0, 1, 1, 1)
        self.cmb_model = QtWidgets.QComboBox(self.layoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cmb_model.sizePolicy().hasHeightForWidth())
        self.cmb_model.setSizePolicy(sizePolicy)
        self.cmb_model.setObjectName("cmb_model")
        self.gridLayout_3.addWidget(self.cmb_model, 1, 1, 1, 1)
        self.btn_add_gif = QtWidgets.QPushButton(self.layoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btn_add_gif.sizePolicy().hasHeightForWidth())
        self.btn_add_gif.setSizePolicy(sizePolicy)
        self.btn_add_gif.setObjectName("btn_add_gif")
        self.gridLayout_3.addWidget(self.btn_add_gif, 2, 1, 1, 1)
        self.tabWidget.addTab(self.tab2, "Input")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.layoutWidget1 = QtWidgets.QWidget(self.tab)
        self.layoutWidget1.setGeometry(QtCore.QRect(0, 10, 311, 221))
        self.layoutWidget1.setObjectName("layoutWidget1")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.layoutWidget1)
        self.gridLayout_4.setContentsMargins(0, 0, 0, 0)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.btn_add_output = QtWidgets.QPushButton(self.layoutWidget1)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btn_add_output.sizePolicy().hasHeightForWidth())
        self.btn_add_output.setSizePolicy(sizePolicy)
        self.btn_add_output.setObjectName("btn_add_output")
        self.gridLayout_4.addWidget(self.btn_add_output, 0, 1, 1, 1)
        self.cmb_output = QtWidgets.QComboBox(self.layoutWidget1)
        self.cmb_output.setObjectName("cmb_output")
        self.gridLayout_4.addWidget(self.cmb_output, 0, 2, 1, 1)
        self.tabWidget.addTab(self.tab, "Output")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.gridLayoutWidget = QtWidgets.QWidget(self.tab_2)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(20, 10, 327, 231))
        self.gridLayoutWidget.setObjectName("gridLayoutWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.setObjectName("gridLayout")
        self.lbl_slider_min_visibility = QtWidgets.QLabel(self.gridLayoutWidget)
        self.lbl_slider_min_visibility.setObjectName("lbl_slider_min_visibility")
        self.gridLayout.addWidget(self.lbl_slider_min_visibility, 0, 2, 1, 1)
        self.h_slider_min_visibility = QtWidgets.QSlider(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.h_slider_min_visibility.sizePolicy().hasHeightForWidth())
        self.h_slider_min_visibility.setSizePolicy(sizePolicy)
        self.h_slider_min_visibility.setMaximum(99)
        self.h_slider_min_visibility.setSliderPosition(70)
        self.h_slider_min_visibility.setOrientation(QtCore.Qt.Horizontal)
        self.h_slider_min_visibility.setTickPosition(QtWidgets.QSlider.TicksAbove)
        self.h_slider_min_visibility.setTickInterval(10)
        self.h_slider_min_visibility.setObjectName("h_slider_min_visibility")
        self.gridLayout.addWidget(self.h_slider_min_visibility, 0, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 3, 0, 1, 1)
        self.label = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 2, 0, 1, 1)
        self.chk_is_move = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.chk_is_move.setChecked(True)
        self.chk_is_move.setObjectName("chk_is_move")
        self.gridLayout.addWidget(self.chk_is_move, 4, 0, 1, 1)
        self.label_2 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 0, 0, 1, 1)
        self.lbl_slider_model_complexity = QtWidgets.QLabel(self.gridLayoutWidget)
        self.lbl_slider_model_complexity.setObjectName("lbl_slider_model_complexity")
        self.gridLayout.addWidget(self.lbl_slider_model_complexity, 2, 2, 1, 1)
        self.chk_is_show_result = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.chk_is_show_result.setChecked(True)
        self.chk_is_show_result.setObjectName("chk_is_show_result")
        self.gridLayout.addWidget(self.chk_is_show_result, 4, 1, 1, 1)
        self.h_slider_model_complexity = QtWidgets.QSlider(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.h_slider_model_complexity.sizePolicy().hasHeightForWidth())
        self.h_slider_model_complexity.setSizePolicy(sizePolicy)
        self.h_slider_model_complexity.setMaximum(2)
        self.h_slider_model_complexity.setSliderPosition(1)
        self.h_slider_model_complexity.setOrientation(QtCore.Qt.Horizontal)
        self.h_slider_model_complexity.setInvertedAppearance(False)
        self.h_slider_model_complexity.setInvertedControls(False)
        self.h_slider_model_complexity.setTickPosition(QtWidgets.QSlider.TicksAbove)
        self.h_slider_model_complexity.setTickInterval(1)
        self.h_slider_model_complexity.setObjectName("h_slider_model_complexity")
        self.gridLayout.addWidget(self.h_slider_model_complexity, 2, 1, 1, 1)
        self.lbl_slider_max_frame_num = QtWidgets.QLabel(self.gridLayoutWidget)
        self.lbl_slider_max_frame_num.setObjectName("lbl_slider_max_frame_num")
        self.gridLayout.addWidget(self.lbl_slider_max_frame_num, 3, 2, 1, 1)
        self.h_slider_max_frame_num = QtWidgets.QSlider(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.h_slider_max_frame_num.sizePolicy().hasHeightForWidth())
        self.h_slider_max_frame_num.setSizePolicy(sizePolicy)
        self.h_slider_max_frame_num.setMinimum(1)
        self.h_slider_max_frame_num.setMaximum(5000)
        self.h_slider_max_frame_num.setSliderPosition(5000)
        self.h_slider_max_frame_num.setOrientation(QtCore.Qt.Horizontal)
        self.h_slider_max_frame_num.setInvertedAppearance(False)
        self.h_slider_max_frame_num.setInvertedControls(False)
        self.h_slider_max_frame_num.setTickPosition(QtWidgets.QSlider.TicksAbove)
        self.h_slider_max_frame_num.setTickInterval(100)
        self.h_slider_max_frame_num.setObjectName("h_slider_max_frame_num")
        self.gridLayout.addWidget(self.h_slider_max_frame_num, 3, 1, 1, 1)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout()
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.label_4 = QtWidgets.QLabel(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setObjectName("label_4")
        self.verticalLayout_2.addWidget(self.label_4)
        self.label_5 = QtWidgets.QLabel(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)
        self.label_5.setObjectName("label_5")
        self.verticalLayout_2.addWidget(self.label_5)
        self.gridLayout.addLayout(self.verticalLayout_2, 1, 0, 1, 1)
        self.h_slider_min_detection_confidence = QtWidgets.QSlider(self.gridLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.h_slider_min_detection_confidence.sizePolicy().hasHeightForWidth())
        self.h_slider_min_detection_confidence.setSizePolicy(sizePolicy)
        self.h_slider_min_detection_confidence.setSliderPosition(50)
        self.h_slider_min_detection_confidence.setOrientation(QtCore.Qt.Horizontal)
        self.h_slider_min_detection_confidence.setTickPosition(QtWidgets.QSlider.TicksAbove)
        self.h_slider_min_detection_confidence.setTickInterval(10)
        self.h_slider_min_detection_confidence.setObjectName("h_slider_min_detection_confidence")
        self.gridLayout.addWidget(self.h_slider_min_detection_confidence, 1, 1, 1, 1)
        self.lbl_slider_min_detection_confidence = QtWidgets.QLabel(self.gridLayoutWidget)
        self.lbl_slider_min_detection_confidence.setObjectName("lbl_slider_min_detection_confidence")
        self.gridLayout.addWidget(self.lbl_slider_min_detection_confidence, 1, 2, 1, 1)
        self.tabWidget.addTab(self.tab_2, "Options")
        self.verticalLayout.addWidget(self.tabWidget)
        self.line1 = QtWidgets.QFrame(self.frame)
        self.line1.setFrameShape(QtWidgets.QFrame.HLine)
        self.line1.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.line1.setObjectName("line1")
        self.verticalLayout.addWidget(self.line1)
        self.btn_convert = QtWidgets.QPushButton(self.frame)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btn_convert.sizePolicy().hasHeightForWidth())
        self.btn_convert.setSizePolicy(sizePolicy)
        self.btn_convert.setObjectName("btn_convert")
        self.verticalLayout.addWidget(self.btn_convert)

        self.retranslateUi(Dialog)
        self.tabWidget.setCurrentIndex(2)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Mediapipe Pose"))
        self.lbl_display2.setText(_translate("Dialog", "Animation GIF"))
        self.lbl_display.setText(_translate("Dialog", "Model Json"))
        self.btn_add_model.setText(_translate("Dialog", "Add Model"))
        self.btn_add_gif.setText(_translate("Dialog", "Add Gif"))
        self.btn_add_output.setText(_translate("Dialog", "Add Output"))
        self.lbl_slider_min_visibility.setText(_translate("Dialog", "70"))
        self.label_3.setText(_translate("Dialog", "Maximum Frame"))
        self.label.setText(_translate("Dialog", "Model Complexity"))
        self.chk_is_move.setText(_translate("Dialog", "Move"))
        self.label_2.setText(_translate("Dialog", "Min Visibility"))
        self.lbl_slider_model_complexity.setText(_translate("Dialog", "1"))
        self.chk_is_show_result.setText(_translate("Dialog", "Show Result"))
        self.lbl_slider_max_frame_num.setText(_translate("Dialog", "5000"))
        self.label_4.setText(_translate("Dialog", "Min Detection"))
        self.label_5.setText(_translate("Dialog", "confidence"))
        self.lbl_slider_min_detection_confidence.setText(_translate("Dialog", "50"))
        self.btn_convert.setText(_translate("Dialog", "Convert"))



class Mixamo(IntEnum):
    Hips = 0
    Spine = auto()
    Spine1 = auto()
    Spine2 = auto()
    Neck = auto()
    Head = auto()
    LeftArm = auto()
    LeftForeArm = auto()
    LeftHand = auto()
    LeftHandThumb1 = auto()
    LeftHandIndex1 = auto()
    LeftHandPinky1 = auto()
    RightArm = auto()
    RightForeArm = auto()
    RightHand = auto()
    RightHandThumb1 = auto()
    RightHandIndex1 = auto()
    RightHandPinky1 = auto()
    LeftUpLeg = auto()
    LeftLeg = auto()
    LeftFoot = auto()
    LeftToeBase = auto()
    RightUpLeg = auto()
    RightLeg = auto()
    RightFoot = auto()
    RightToeBase = auto()

def get_3d_len(left):
    return math.sqrt((left["x"])**2 + (left["y"])**2 + (left["z"])**2)

def set_axes(ax, azim=10, elev=10, xrange=1.0, yrange=1.0, zrange=1.0):
    ax.set_xlabel("Z")
    ax.set_ylabel("X")
    ax.set_zlabel("Y")
    ax.set_title('Vector')
    if xrange > 0.0:
        ax.set_xlim(-xrange, xrange)
        ax.set_ylim(-yrange, yrange)
        ax.set_zlim(-zrange, zrange)
    ax.view_init(elev=elev, azim=azim)
    
def get_dot(vec_list, group_lists):
    dots = []
    for group_list in group_lists:
        dot_group = {
            'x': [],
            'y': [],
            'z': []
        }
        for idx in group_list:
            dot_group['x'].append(vec_list[idx][2])
            dot_group['y'].append(vec_list[idx][0])
            dot_group['z'].append(vec_list[idx][1])
        dots.append(dot_group)
    return dots
  
def draw_list2(fig, vec_list=[], group_lists=[[]], azim=10, range=1.0):
    ax1 = plt.axes(projection='3d')
    set_axes(ax1, elev=10, azim=azim, xrange=range, yrange=range, zrange=range)
    dots = get_dot(vec_list, group_lists)
    for dot in dots:
        ax1.plot(dot['x'], dot['y'], dot['z'], marker='o')
    
    fig.canvas.draw()

def find_bones(bones, name):
    for bone in bones:
        if bone["name"] == name:
            return bone
    return None    

def find_hips(pixel3d_json):
    return find_pixel3d_json(pixel3d_json, Mixamo.Hips.name)


def find_pixel3d_json(pixel3d_json, name):
    if pixel3d_json["name"] == name:
        return [True, pixel3d_json]
    else:
        for child in pixel3d_json["child"]:
            is_find, result = find_hips(child)
            if is_find:
                return [is_find, result]
        return [False, None]
def get_name_idx_map():    
    mediapipe_names = [
    "nose", 
    "left_eye_inner", "left_eye", "left_eye_outer", "right_eye_inner", "right_eye", "right_eye_outer", "left_ear", "right_ear", "mouth_left", "mouth_right", "left_shoulder", "right_shoulder", "left_elbow", "right_elbow", "left_wrist", "right_wrist", "left_pinky", "right_pinky", "left_index", "right_index", "left_thumb", "right_thumb", "left_hip", "right_hip", "left_knee", "right_knee", "left_ankle", "right_ankle", "left_heel", "right_heel", "left_foot_index", "right_foot_index"]

    name_idx_map = {}
    for idx in range(0, len(mediapipe_names)):
        name_idx_map[mediapipe_names[idx]] = idx
    return name_idx_map

def get_mixamo_names():
    return [
        ['Hips', 0, -1],  # left hip <->right hip
        ['Spine', 1, 0],
        ['Spine1', 2, 1],
        ['Spine2', 3, 2],

        ['Neck', 4, 3],  # left_shoulder <-> right_shoulder
        ['Head', 5, 4],  # left_ear <-> right_ear

        ['LeftArm', 6, 3, "left_shoulder"],
        ['LeftForeArm', 7, 6, "left_elbow"],
        ['LeftHand', 8, 7, "left_wrist"],
        ['LeftHandThumb1', 9, 8, "left_thumb"],
        ['LeftHandIndex1', 10, 8, "left_index"],
        ['LeftHandPinky1', 11, 8, "left_pinky"],

        ['RightArm', 12, 3, "right_shoulder"],
        ['RightForeArm', 13, 12, "right_elbow"],
        ['RightHand', 14, 13, "right_wrist"],
        ['RightHandThumb1', 15, 14, "right_thumb"],
        ['RightHandIndex1', 16, 14, "right_index"],
        ['RightHandPinky1', 17, 14, "right_pinky"],

        ['LeftUpLeg', 18, 0, "left_hip"],
        ['LeftLeg', 19, 18, "left_knee"],
        ['LeftFoot', 20, 19, "left_ankle"],
        ['LeftToeBase', 21, 20, "left_foot_index"],

        ['RightUpLeg', 22, 0, "right_hip"],
        ['RightLeg', 23, 22, "right_knee"],
        ['RightFoot', 24, 23, "right_ankle"],
        ['RightToeBase', 25, 24, "right_foot_index"]
    ]

def get_mixamo_name_idx_map():
    mixamo_names = get_mixamo_names()
    mixamo_name_idx_map = {}
    for name in mixamo_names:
        mixamo_name_idx_map[name[0]] = name[1]
    return mixamo_name_idx_map

def get_mixamo_name_mediapipe_name_map():
    mixamo_name_mediapipe_name_map = {}
    mixamo_names = get_mixamo_names()
    for idx in range(6, len(mixamo_names)):
        mixamo_name_mediapipe_name_map[mixamo_names[idx]
                                       [0]] = mixamo_names[idx][3]
    return mixamo_name_mediapipe_name_map

##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################

class Gizmo:
    def __init__(self, r=glm.vec3(0.0, 0.0, 0.0),
                 x=glm.vec3(1.0, 0.0, 0.0),
                 y=glm.vec3(0.0, 1.0, 0.0),
                 z=glm.vec3(0.0, 0.0, 1.0)):
        self.r = r
        self.x = x
        self.y = y
        self.z = z

    def set_origin(self, r=glm.vec3(0.0, 0.0, 0.0)):
        self.r = r

    def rotate(self, transform_mat):
        r = transform_mat * self.r
        x = transform_mat * self.x
        y = transform_mat * self.y
        z = transform_mat * self.z
        return Gizmo(r, x, y, z)

    def calc_rotation_matrix(self, world_start, world_end):
        local_point1 = glm.normalize(self.get_local_pos(world_start))
        local_point2 = glm.normalize(self.get_local_pos(world_end))
        return glm.rotate(glm.mat4(1.0),
                          glm.acos(glm.dot(local_point1, local_point2)),
                          glm.cross(local_point1, local_point2))

    def calc_roll(self, world_start, world_end):
        local_point1 = glm.normalize(self.get_local_pos(world_start))
        local_point2 = glm.normalize(self.get_local_pos(world_end))
        local_point1.x = 0
        local_point2.x = 0
        return glm.rotate(glm.mat4(1.0),
                          glm.acos(glm.dot(local_point1, local_point2)),
                          glm.cross(local_point1, local_point2))

    # def calc_roll(self, world_start, world_end):

    def get_origin(self):
        return self.r

    def get_local_pos(self, world_pos):
        b = world_pos - self.r
        A = glm.mat3(self.x, self.y, self.z)
        x = glm.inverse(A)*b
        if math.isnan(x.x):
            x.x = 0.0
        if math.isnan(x.y):
            x.y = 0.0
        if math.isnan(x.z):
            x.z = 0.0
        return x
def find_2d_angle(cx, cy, ex, ey):
    dy = ey - cy
    dx = ex - cx
    theta = math.atan2(dy, dx)
    return theta


def pixel3d_json_to_glm_vec(pixel3d_json):
    return glm.vec3(pixel3d_json['x'], pixel3d_json['y'], pixel3d_json['z'])


def pixel3d_json_to_glm_quat(pixel3d_json):
    return glm.quat(w=pixel3d_json['w'], x=pixel3d_json['x'], y=pixel3d_json['y'], z=pixel3d_json['z'])


def glm_vec3_to_json(vec):
    return {"x": vec.x, "y": vec.y, "z": vec.z}


def glm_quat_to_json(quat):
    return {"w": quat.w, "x": quat.x, "y": quat.y, "z": quat.z}


def check_quat_isnan(quat):
    if math.isnan(quat.x):
        quat.x = 0.0
    if math.isnan(quat.w):
        quat.w = 1.0
    if math.isnan(quat.y):
        quat.y = 0.0
    if math.isnan(quat.z):
        quat.z = 0.0


def check_vec3_isnan(vec, num=0.0):
    if math.isnan(vec.x):
        vec.x = num
    if math.isnan(vec.y):
        vec.y = num
    if math.isnan(vec.z):
        vec.z = num


def decompose(matrix: glm.mat4):
    scale = glm.vec3()
    rotation = glm.quat()
    translation = glm.vec3()
    skew = glm.vec3()
    perspective = glm.vec4()
    glm.decompose(matrix, scale, rotation, translation, skew, perspective)
    check_quat_isnan(rotation)
    check_vec3_isnan(scale, 1.0)
    check_vec3_isnan(translation, 0.0)
    return [translation, rotation, scale]


def calc_transform(position, rotation, scale):
    pos = glm.translate(glm.mat4(1.0), position)
    rot = pos * glm.mat4(rotation)
    return glm.scale(rot, scale)
def calc_hip_transform_1(mixamo_list, hips_gizmo, left_up_leg_gizmo, spine_gizmo):
    transform = hips_gizmo.calc_rotation_matrix(
        left_up_leg_gizmo.get_origin(), mixamo_list[Mixamo.LeftUpLeg])

    hip_gizmo_r = hips_gizmo.rotate(transform)

    roll = hip_gizmo_r.calc_roll(
        transform*spine_gizmo.get_origin(), mixamo_list[Mixamo.Spine])
    return transform * roll


def calc_hip_transform(mixamo_list, hips_node, left_up_leg_node, spine_node):
    hip_gizmo = hips_node.get_gizmo()
    left_up_leg_gizmo = left_up_leg_node.get_gizmo(hips_node.get_transform())
    spine_gizmo = spine_node.get_gizmo(hips_node.get_transform())

    return calc_hip_transform_1(mixamo_list, hip_gizmo, left_up_leg_gizmo, spine_gizmo)

class ModelNode:
    def __init__(self, gizmo=Gizmo()):
        self.child = []
        self.gizmo = gizmo
        self.name = ""
        self.position = glm.vec3(x=0.0, y=0.0, z=0.0)
        self.scale = glm.vec3(x=1.0, y=1.0, z=1.0)
        self.rotate = glm.quat(w=1.0, x=0.0, y=0.0, z=0.0)

        self.tmp_transform = glm.mat4(1.0)

    def find_node(self, name):
        if self.name == name:
            return [True, self]
        for child in self.child:
            is_find, node = child.find_node(name)
            if is_find:
                return [True, node]
        return [False, None]

    def set_pixel3d(self, pixel3d_node_json):
        self.name = pixel3d_node_json["name"]
        self.position = pixel3d_json_to_glm_vec(pixel3d_node_json["position"])
        self.rotate = pixel3d_json_to_glm_quat(pixel3d_node_json["rotation"])
        self.scale = pixel3d_json_to_glm_vec(pixel3d_node_json["scale"])
        for child in pixel3d_node_json["child"]:
            new_node = ModelNode()
            new_node.set_pixel3d(child)
            self.child.append(new_node)

    def set_mixamo(self, pix3d_node_json, mixamo_idx_map, before_transform=None):
        self.name = pix3d_node_json["name"]
        self.position = pixel3d_json_to_glm_vec(
            pix3d_node_json["position"])
        self.rotate = pixel3d_json_to_glm_quat(pix3d_node_json["rotation"])
        self.scale = pixel3d_json_to_glm_vec(pix3d_node_json["scale"])
        if before_transform != None:
            self.position, self.rotate, self.scale = decompose(
                before_transform*self.get_transform())
        if self.name == Mixamo.LeftUpLeg.name or self.name == Mixamo.RightUpLeg.name:
            self.position.y = 0
            self.position.z = 0

        # find child
        childlist = pix3d_node_json["child"]
        transform_list = [None for i in range(len(childlist))]
        transform_list_idx = -1

        for child in childlist:
            transform_list_idx += 1
            if child['name'] in mixamo_idx_map:
                new_node = ModelNode()
                new_node.set_mixamo(child, mixamo_idx_map, copy.deepcopy(
                    transform_list[transform_list_idx]))
                self.child.append(new_node)
            else:
                for child_of_child in child["child"]:
                    childlist.append(child_of_child)
                    position = pixel3d_json_to_glm_vec(child["position"])
                    rotation = pixel3d_json_to_glm_quat(child["rotation"])
                    scale = pixel3d_json_to_glm_vec(child["scale"])
                    transform = calc_transform(position, rotation, scale)
                    transform_list.append(transform)

    def normalize_spine(self, parent_node=None, parent_transform=glm.mat4(1.0)):
        if Mixamo.Spine.name in self.name or self.name == Mixamo.LeftArm.name or self.name == Mixamo.RightArm.name or self.name == Mixamo.Neck.name:
            self.position.z = 0  # local_pos.z


        for child in self.child:
            child.normalize_spine(
                parent_node=self, parent_transform=self.tmp_transform*self.get_transform())

    def normalize(self, mixamo_list, mixamo_idx_map, len=0.0):
        if self.name == "Hips":
            self.position = mixamo_list[mixamo_idx_map[self.name]]
        else:
            n_position = glm.normalize(self.position)
            self.position = len*n_position

        for child in self.child:

            a = mixamo_list[mixamo_idx_map[self.name]]
            b = mixamo_list[mixamo_idx_map[child.name]]
            new_len = glm.distance(a, b)
            child.normalize(mixamo_list, mixamo_idx_map, new_len)

    def find_child(self, name):
        for child in self.child:
            if child.name == name:
                return child
        return None

    def calc_animation(self, mixamo_list, mixamo_idx_map, parent_transform=glm.mat4(1.0), world_mixamo_adjust=glm.vec3(0.0, 0.0, 0.0)):
        self.tmp_transform = glm.mat4(1.0)
        if self.name == "Hips":
            self.tmp_transform = calc_hip_transform(mixamo_list,
                                                    self,
                                                    self.find_child(
                                                        Mixamo.LeftUpLeg.name),
                                                    self.find_child(Mixamo.Spine.name))
        elif len(self.child) > 0:
            current_gizmo = self.get_gizmo(parent_transform)
            target_node = self.child[0]
            target_gizmo = target_node.get_gizmo(
                parent_transform*self.get_transform())
            target_vec = world_mixamo_adjust + \
                mixamo_list[mixamo_idx_map[target_node.name]]
            self.tmp_transform = current_gizmo.calc_rotation_matrix(
                target_gizmo.get_origin(), target_vec)

        for child in self.child:
            adjust_vec = child.get_gizmo(
                parent_transform*self.get_transform()*self.tmp_transform).get_origin()
            adjust_vec -= (world_mixamo_adjust +
                           mixamo_list[mixamo_idx_map[child.name]])
            child.calc_animation(mixamo_list, mixamo_idx_map,
                                 parent_transform*self.get_transform()*self.tmp_transform, world_mixamo_adjust=adjust_vec)

    def get_transform(self):
        return calc_transform(self.position, self.rotate, self.scale)

    def get_gizmo(self, parent_transform=glm.mat4(1.0), is_apply_transform=True):
        if is_apply_transform:
            return self.gizmo.rotate(parent_transform*self.get_transform())
        else:
            return self.gizmo.rotate(parent_transform)

    def get_gizmo_apply_tmp(self, parent_transform=glm.mat4(1.0)):
        return self.gizmo.rotate(parent_transform*self.get_transform()*self.tmp_transform)

    def get_vec_and_group_list(self, result_vec_list, result_group_list, parent_transform=glm.mat4(1.0),  group_list=None, is_apply_tmp_transform=False):
        if is_apply_tmp_transform:
            result_vec_list.append(self.get_gizmo_apply_tmp(
                parent_transform).get_origin())
        else:
            result_vec_list.append(self.get_gizmo(
                parent_transform).get_origin())
        if group_list == None:
            group_list = []
        group_list.append(len(result_vec_list) - 1)

        if len(self.child) == 0:
            result_group_list.append(copy.deepcopy(group_list))
            group_list.clear()
            return

        for child in self.child:
            if is_apply_tmp_transform:
                child.get_vec_and_group_list(
                    result_vec_list, result_group_list, group_list=group_list, parent_transform=copy.deepcopy(parent_transform*self.get_transform() * self.tmp_transform), is_apply_tmp_transform=is_apply_tmp_transform)
            else:
                child.get_vec_and_group_list(
                    result_vec_list, result_group_list, group_list=group_list, parent_transform=copy.deepcopy(parent_transform*self.get_transform()), is_apply_tmp_transform=is_apply_tmp_transform)

    def tmp_to_json(self, bones_json, visibility_list, mixamo_idx_map, min_visibility=0.6):
        [t, r, s] = decompose(self.tmp_transform)
        visibility = visibility_list[mixamo_idx_map[self.name]]
        if visibility >= min_visibility and (not (r.w == 1.0 and r.x == 0.0 and r.y == 0.0 and r.z == 0.0)):
            bone_json = {
                "name": self.name,
                "rotation": glm_quat_to_json(r),
                "position": glm_vec3_to_json(t),
                "scale": glm_vec3_to_json(s)
            }
            bones_json["bones"].append(bone_json)
        for child in self.child:
            child.tmp_to_json(bones_json, visibility_list,
                              mixamo_idx_map, min_visibility)
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
##################################################################################################################################################################################
def mediapipe_to_mixamo(mixamo_bindingpose_path, 
                        video_path, 
                        is_hips_move = False, 
                        min_visibility = 0.5, 
                        min_detection_confidence = 0.5, 
                        model_complexity = 1, 
                        static_image_mode = False, 
                        max_frame_num = 5000,
                        is_show_result = False):
    mm_name_idx_map  = get_mixamo_name_idx_map()
    mixamo_json = None
    with open(mixamo_bindingpose_path) as f:
        mixamo_json = json.load(f)
    is_find, hip_node = find_hips(mixamo_json["node"])
    if not is_find:
        return [False, None]

    print(video_path)
    
    cap = cv2.VideoCapture(video_path)
    fps = cap.get(cv2.CAP_PROP_FPS)

    anim_result_json = {
        "fileName": os.path.basename(video_path),
        "duration": 0,
        "width":  cap.get(cv2.CAP_PROP_FRAME_WIDTH),
        "height": cap.get(cv2.CAP_PROP_FRAME_HEIGHT), 
        "ticksPerSecond": math.trunc(fps),
        "frames": [
        ]        
    }
    
    try:
        root_node = ModelNode()
        root_node.set_mixamo(hip_node, mm_name_idx_map)
        root_node.normalize_spine()

        mediapipe_to_mixamo2(anim_result_json, 
                                cap, 
                                mixamo_json,
                                root_node, 
                                is_hips_move=is_hips_move, 
                                min_visibility=min_visibility, 
                                min_detection_confidence=min_detection_confidence, 
                                model_complexity=model_complexity, 
                                static_image_mode=static_image_mode, 
                                max_frame_num=max_frame_num,
                                is_show_result= is_show_result)
        anim_result_json["duration"]= anim_result_json["frames"][-1]["time"]

    except Exception as e:
        print(e)
        if cap.isOpened():
            cap.release()
        return [False, None]
    if cap.isOpened():
        cap.release()
    return [True, anim_result_json]


def mediapipe_to_mixamo2(anim_result_json, 
                         cap, 
                         mixamo_bindingpose_json, 
                         mixamo_bindingpose_root_node, 
                         is_hips_move = False, 
                         min_visibility = 0.5, 
                         min_detection_confidence = 0.5, 
                         model_complexity = 1,
                         static_image_mode = False, 
                         max_frame_num = 5000,
                         is_show_result = False):
    # init dicts
    mp_name_idx_map = get_name_idx_map()
    mm_mp_map = get_mixamo_name_mediapipe_name_map()
    mm_name_idx_map  = get_mixamo_name_idx_map()
    mp_idx_mm_idx_map = dict()
    for mm_name in mm_mp_map.keys():
        mp_name = mm_mp_map[mm_name]
        mp_idx = mp_name_idx_map[mp_name]
        mm_idx = mm_name_idx_map[mm_name]
        mp_idx_mm_idx_map[mp_idx] = mm_idx
    
    # for hips move var
    _, model_right_up_leg = find_pixel3d_json(mixamo_bindingpose_json["node"], Mixamo.RightUpLeg.name)
    model_scale = 100
    if _ != False:
        model_scale = get_3d_len(model_right_up_leg["position"])
    origin = None
    factor = 1.0
    width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
    height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)

    frame_num = -1
    plt.ion()
    plt.close()
    fig = None
    if is_show_result: 
        fig = plt.figure()
        plt.show()
    
    # init mediapipe
    mp_pose = mp.solutions.pose
    mp_drawing = mp.solutions.drawing_utils 
    try:
        with mp_pose.Pose(
            static_image_mode=static_image_mode,
                        min_detection_confidence=min_detection_confidence, 
                        model_complexity=model_complexity
        ) as pose:
            while cap.isOpened():

                success, cap_image = cap.read()
                frame_num += 1
                if not success or max_frame_num < frame_num :
                    break

                cap_image, glm_list, visibility_list, hip2d_left, hip2d_right = detect_pose_to_glm_pose(mp_pose, mp_drawing, cap_image, pose, mp_idx_mm_idx_map)
                if glm_list[0] != None:
                    bones_json = {
                       "time": frame_num,
                       "bones": []
                    } 
                    mixamo_bindingpose_root_node.normalize(glm_list, mm_name_idx_map)
                    mixamo_bindingpose_root_node.calc_animation(glm_list, mm_name_idx_map)
                    mixamo_bindingpose_root_node.tmp_to_json(bones_json, visibility_list, mm_name_idx_map, min_visibility)
                    anim_result_json["frames"].append(bones_json)
                    if is_show_result:
                        rg = []
                        rv = []
                        mixamo_bindingpose_root_node.get_vec_and_group_list(rv, rg, is_apply_tmp_transform= True)
                        plt.clf()
                        draw_list2(fig, rv, rg)
                    if is_hips_move:
                        hip2d_left.x *=  width
                        hip2d_left.y *=  height
                        hip2d_left.z *=  width
                        hip2d_right.x *= width
                        hip2d_right.y *= height
                        hip2d_right.z *= width
                        if origin == None:
                           origin = avg_vec3(hip2d_left, hip2d_right)
                           hips2d_scale = glm.distance(hip2d_left, hip2d_right)
                           factor = model_scale/hips2d_scale
                        else:
                            hips_bone_json = find_bones(anim_result_json["frames"][-1]["bones"], Mixamo.Hips.name)
                            if hips_bone_json != None:
                                set_hips_position(hips_bone_json["position"], origin, avg_vec3(hip2d_left, hip2d_right),  factor)

                cv2.imshow('MediaPipe pose', cap_image)

                if cv2.waitKey(5) & 0xFF == 27:
                    break
        cap.release()
        # plt.close(fig)
        cv2.destroyAllWindows()    

    except Exception as e:
        print(e)
        # plt.close(fig)
        if cap.isOpened():
            cap.release()
            cv2.destroyAllWindows()
        

def detect_pose_to_glm_pose(mp_pose, mp_drawing, image, pose, mp_idx_mm_idx_map):
    # Create a copy of the input image.
    output_image = image.copy()
    
    image.flags.writeable = False

    # Convert the image from BGR into RGB format.
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Perform the Pose Detection.
    results = pose.process(image_rgb)
    
    image.flags.writeable = True

    # Initialize a list to store the detected landmarks.
    glm_list = [None]*26
    visibility_list = [None]*26
    hip2d_left, hip2d_right = glm.vec3(0.0, 0.0, 0.0), glm.vec3(0.0, 0.0, 0.0)

    if results.pose_world_landmarks:
        landmark = results.pose_world_landmarks.landmark

        glm_list[Mixamo.Hips] = avg_vec3(landmark[mp_pose.PoseLandmark.LEFT_HIP], landmark[mp_pose.PoseLandmark.RIGHT_HIP])
        visibility_list[Mixamo.Hips] = (landmark[mp_pose.PoseLandmark.LEFT_HIP].visibility +  landmark[mp_pose.PoseLandmark.RIGHT_HIP].visibility)*0.5
        glm_list[Mixamo.Neck]  = avg_vec3(landmark[mp_pose.PoseLandmark.LEFT_SHOULDER], landmark[mp_pose.PoseLandmark.RIGHT_SHOULDER])
        visibility_list[Mixamo.Neck] = (landmark[mp_pose.PoseLandmark.LEFT_SHOULDER].visibility +  landmark[mp_pose.PoseLandmark.RIGHT_SHOULDER].visibility)*0.5
        glm_list[Mixamo.Spine1] = avg_vec3(glm_list[Mixamo.Hips], glm_list[Mixamo.Neck])
        visibility_list[Mixamo.Spine1] = (visibility_list[Mixamo.Hips] + visibility_list[Mixamo.Neck])*0.5
        glm_list[Mixamo.Spine] = avg_vec3(glm_list[Mixamo.Hips], glm_list[Mixamo.Spine1])
        visibility_list[Mixamo.Spine] = (visibility_list[Mixamo.Hips] + visibility_list[Mixamo.Spine1])*0.5
        glm_list[Mixamo.Spine2] = avg_vec3(glm_list[Mixamo.Spine1], glm_list[Mixamo.Neck])
        visibility_list[Mixamo.Spine2] = (visibility_list[Mixamo.Spine1] + visibility_list[Mixamo.Neck])*0.5
        glm_list[Mixamo.Head]  = avg_vec3(landmark[mp_pose.PoseLandmark.LEFT_EAR], landmark[mp_pose.PoseLandmark.RIGHT_EAR])
        visibility_list[Mixamo.Head] = (landmark[mp_pose.PoseLandmark.LEFT_EAR].visibility +  landmark[mp_pose.PoseLandmark.RIGHT_EAR].visibility)*0.5
        glm_list[Mixamo.Neck].y *= -1
        glm_list[Mixamo.Neck].z *= -1
        glm_list[Mixamo.Spine].y *= -1
        glm_list[Mixamo.Spine].z *= -1
        glm_list[Mixamo.Spine1].y *= -1
        glm_list[Mixamo.Spine1].z *= -1
        glm_list[Mixamo.Spine2].y *= -1
        glm_list[Mixamo.Spine2].z *= -1
        glm_list[Mixamo.Head].y *= -1
        glm_list[Mixamo.Head].z *= -1
        for mp_idx in mp_idx_mm_idx_map.keys():
            mm_idx = mp_idx_mm_idx_map[mp_idx]
            glm_list[mm_idx] = glm.vec3(landmark[mp_idx].x, -landmark[mp_idx].y, -landmark[mp_idx].z)
            visibility_list[mm_idx] = landmark[mp_idx].visibility


    image_height, image_width, _ = image.shape

    # calculate hips2d
    if results.pose_landmarks:
        landmark = results.pose_landmarks.landmark
        hip2d_left.x = landmark[mp_pose.PoseLandmark.LEFT_HIP].x
        hip2d_left.y = landmark[mp_pose.PoseLandmark.LEFT_HIP].y
        hip2d_left.z = landmark[mp_pose.PoseLandmark.LEFT_HIP].z
        hip2d_right = glm.vec3(landmark[mp_pose.PoseLandmark.RIGHT_HIP].x,landmark[mp_pose.PoseLandmark.RIGHT_HIP].y,landmark[mp_pose.PoseLandmark.RIGHT_HIP].z)

    mp_drawing.draw_landmarks(image=output_image, landmark_list=results.pose_landmarks,
                              connections=mp_pose.POSE_CONNECTIONS)

    return output_image, glm_list, visibility_list, hip2d_left, hip2d_right

def avg_vec3(v1, v2):
    v3 = glm.vec3((v1.x + v2.x)* 0.5, 
                 (v1.y + v2.y)* 0.5, 
                 (v1.z + v2.z)* 0.5)
    return v3

def set_hips_position(hips_bone_json, origin_hips, current_hips, factor):
    x = (current_hips.x - origin_hips.x)* factor
    y = (current_hips.y - origin_hips.y) * factor
    z = (current_hips.z - origin_hips.z) * factor
    hips_bone_json["x"] = x
    hips_bone_json["y"] = -y
    hips_bone_json["z"] = z

# %%
class WindowClass(QMainWindow, Ui_Dialog):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.statusBar().showMessage('Ready')

        self.setFixedWidth(self.frameGeometry().width())
        self.setFixedHeight(self.frameGeometry().height())
        self.is_converting = False

        self.btn_add_model.adjustSize()
        # 버튼에 기능을 연결하는 코드
        self.btn_add_model.clicked.connect(self.add_model_path)
        self.btn_add_gif.clicked.connect(self.add_gif_path)
        self.btn_add_output.clicked.connect(self.add_output_path)
        self.btn_convert.clicked.connect(self.convert)
        # self.cmb_gif.currentIndexChanged.connect(self.show_gif)
        self.h_slider_min_visibility.valueChanged.connect(
            self.set_lbl_min_visibility)
        self.h_slider_model_complexity.valueChanged.connect(
            self.set_lbl_model_complexity)
        self.h_slider_max_frame_num.valueChanged.connect(
            self.set_lbl_max_frame_num)
        self.h_slider_min_detection_confidence.valueChanged.connect(
            self.set_lbl_min_detection_confidence)

    def set_lbl_min_visibility(self):
        self.lbl_slider_min_visibility.setText(
            str(self.h_slider_min_visibility.value()))

    def set_lbl_model_complexity(self):
        self.lbl_slider_model_complexity.setText(
            str(self.h_slider_model_complexity.value()))

    def set_lbl_max_frame_num(self):
        self.lbl_slider_max_frame_num.setText(
            str(self.h_slider_max_frame_num.value()))

    def set_lbl_min_detection_confidence(self):
        self.lbl_slider_min_detection_confidence.setText(
            str(self.h_slider_min_detection_confidence.value()))



    def add_model_path(self):
        self.add_cmb_item_from_dialog(
            "Add Model Json File", "./", "Json (*.json)", self.cmb_model)

    def add_gif_path(self):
        self.add_cmb_item_from_dialog(
            "Add Animation GIF File", "./", "GIF (*.gif);; MP4 (*.mp4);; AVI (*.avi);; All files (*.*)", self.cmb_gif)

    def add_output_path(self):
        self.add_cmb_item_from_dialog(
            "Select Output File", "./", "Json (*.json)", self.cmb_output, is_save=True)

    def convert(self):
        if self.is_converting:
            return
        self.statusBar().showMessage('Converting...')
        if self.cmb_model.currentIndex() == -1 or self.cmb_gif.currentIndex() == -1 or self.cmb_output.currentIndex() == -1:
            self.statusBar().showMessage('Please try again')
            return
        
        try:

            self.is_converting = True
            model_path = self.cmb_model.currentText()
            gif_path = self.cmb_gif.currentText()
            output_path = self.cmb_output.currentText()
            max_frame_num = self.h_slider_max_frame_num.value()
            model_complexity = self.h_slider_model_complexity.value()
            min_visibility = self.h_slider_min_visibility.value()/100.0
            max_frame_num = self.h_slider_max_frame_num.value()
            min_detection_confidence = self.h_slider_min_detection_confidence.value()/100.0
            is_move_hips = self.chk_is_move.isChecked()
            is_show_result = self.chk_is_show_result.isChecked()

            _, anim_json = mediapipe_to_mixamo(model_path, 
                                gif_path, 
                                is_move_hips, 
                                min_visibility= min_visibility, 
                                max_frame_num= max_frame_num,
                                model_complexity = model_complexity,
                                is_show_result=is_show_result,
                                min_detection_confidence=min_detection_confidence)
            with open(output_path, 'w') as f:
                json.dump(anim_json, f, indent=2)
            self.statusBar().showMessage('Success!')
            self.is_converting = False

        except Exception as e:
            print(e)
            self.statusBar().showMessage('Error! ' + str(e))
            self.is_converting = False
            return

    def add_cmb_item(self, item, cmb):
        cmb.addItem(item)
        cmb.setCurrentIndex(cmb.count() - 1)

    def add_cmb_item_from_dialog(self, title, path, filter, cmb, is_save=False):
        fname = self.show_dialog(title, path, filter, is_save)
        if fname != '':
            self.add_cmb_item(fname, cmb)

    # filter: "Images (*.png *.xpm .jpg);;Text files (.txt);;XML files (*.xml)"
    def show_dialog(self, title, path, filter, is_save=False):
        if not is_save:
            fname = QFileDialog.getOpenFileName(self, title, path, filter)
        else:
            fname = QFileDialog.getSaveFileName(self, title, path, filter)
        return fname[0]

# if __name__ == '__main__':
    # parser = argparse.ArgumentParser(description='Mediapipe To Mixamo')
    # parser.add_argument(
    #     '--arg1',  ,help='model binding pose json data (pixel3d: Export model)', default=None)
    # parser.add_argument('--arg2', help='output path', default=None)

def execc():

    # args = parser.parse_args()
    # model_path = os.path.abspath(args.arg1)
    # output_path = os.path.abspath(args.arg2)

    app = QApplication(sys.argv)
    myWindow = WindowClass()
    # if(model_path != None):
    #     myWindow.add_cmb_item(model_path, myWindow.cmb_model)
    # if(output_path != None):
    #     myWindow.add_cmb_item(output_path, myWindow.cmb_output)
    myWindow.show()
    app.exec_()