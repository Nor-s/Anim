import json
import cv2
import glm
import math
import matplotlib
import os
from .mixamo import Mixamo
from .model_node import ModelNode
import copy

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
    ax1 = matplotlib.pyplot.axes(projection='3d')
    set_axes(ax1, elev=10, azim=azim, xrange=range, yrange=range, zrange=range)
    dots = get_dot(vec_list, group_lists)
    for dot in dots:
        ax1.plot(dot['x'], dot['y'], dot['z'], marker='o')

    fig.canvas.draw()


def draw_list3(fig, vec_list=[],vec2_list=[], group_lists=[[]], azim=10, range=1.0):
    ax1 =  matplotlib.pyplot.axes(projection='3d') #fig.add_subplot(1, 2, 1, projection='3d') #matplotlib.pyplot.axes(projection='3d')
    set_axes(ax1, elev=10, azim=azim, xrange=range, yrange=range, zrange=range)
    dots = get_dot(vec_list, group_lists)
    for dot in dots:
        ax1.plot(dot['x'], dot['y'], dot['z'], marker='.')

    dots2 = get_dot(vec2_list, group_lists)
    for dot in dots2:
        ax1.plot(dot['x'], dot['y'], dot['z'], marker='+', color='r')

    # plt.show()
    fig.canvas.draw()


def find_bones(bones, name):
    for bone in bones:
        idx = bone["name"].find(":")
        bone_name = bone["name"][idx+1:]
        if bone_name == name:
            return bone
    return None

def find_model_json(model_json, name):
    idx = model_json["name"].find(":")
    model_name = model_json["name"][idx+1:]
    
    if  model_name == name:
        return [True, model_json]
    else:
        for child in model_json["child"]:
            is_find, result = find_model_json(child, name)
            if is_find:
                return [is_find, result]
        return [False, None]


def get_name_idx_map():
    mediapipe_names = [
        "nose",
        "left_eye_inner",
        "left_eye", 
        "left_eye_outer",
        "right_eye_inner", 
        "right_eye", 
        "right_eye_outer", 
        "left_ear", 
        "right_ear", 
        "mouth_left",
        "mouth_right", 
        "left_shoulder", 
        "right_shoulder",
        "left_elbow", 
        "right_elbow", 
        "left_wrist", 
        "right_wrist", 
        "left_pinky",
        "right_pinky", 
        "left_index", 
        "right_index", 
        "left_thumb",
        "right_thumb", 
        "left_hip", 
        "right_hip", 
        "left_knee", 
        "right_knee", 
        "left_ankle", 
        "right_ankle", 
        "left_heel", 
        "right_heel", 
        "left_foot_index", 
        "right_foot_index"]

    name_idx_map = {}
    for idx in range(0, len(mediapipe_names)):
        name_idx_map[mediapipe_names[idx]] = idx
    return name_idx_map



# [Mixamo name, idx, parent_idx, mediapipe name]
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
    mm_name_mp_name_map = {}
    mixamo_names = get_mixamo_names()
    for idx in range(6, len(mixamo_names)):
        mm_name_mp_name_map[mixamo_names[idx][0]] = mixamo_names[idx][3]
    return mm_name_mp_name_map

def mediapipe_to_mixamo(mp_manager,
                        mixamo_dict_string,
                        video_path):
    mm_name_idx_map = get_mixamo_name_idx_map()
    mixamo_json = None
    print(video_path)
    # with open(mixamo_dict_string) as f:
        # mixamo_json = json.load(f)
    mixamo_json = json.loads(mixamo_dict_string)
    is_find, hip_node = find_model_json(mixamo_json["node"], Mixamo.Hips.name)
    if not is_find:
        return [False, None]

    cap = cv2.VideoCapture(video_path)
    fps = cap.get(cv2.CAP_PROP_FPS)
    time_factor = 1.0
    if mp_manager.fps > 0:
        time_factor = mp_manager.fps/fps
        fps = mp_manager.fps
    anim_result_json = {
        "fileName": os.path.basename(video_path),
        "duration": 0,
        "width":  cap.get(cv2.CAP_PROP_FRAME_WIDTH),
        "height": cap.get(cv2.CAP_PROP_FRAME_HEIGHT),
        "ticksPerSecond": fps,
        "frames": [
        ]
    }

    try:
        root_node = ModelNode()
        root_node.set_mixamo(hip_node, mm_name_idx_map)
        root_node.normalize_spine()

        mediapipe_to_mixamo2(mp_manager,
                             anim_result_json,
                             cap,
                             mixamo_json,
                             root_node,
                             time_factor)
        anim_result_json["duration"] = anim_result_json["frames"][-1]["time"]

        print(f"duration: {anim_result_json['duration']}")

    except Exception as e:
        print(e)
        if cap.isOpened():
            cap.release()
        return [False, None]
    if cap.isOpened():
        cap.release()
    return [True, anim_result_json]


def mediapipe_to_mixamo2(mp_manager,
                         anim_result_json,
                         cap,
                         mixamo_bindingpose_json,
                         mixamo_bindingpose_root_node,
                         time_factor):
    # init dicts
    mp_name_idx_map = get_name_idx_map()
    mm_mp_map = get_mixamo_name_mediapipe_name_map()
    mm_name_idx_map = get_mixamo_name_idx_map()
    mp_idx_mm_idx_map = dict()
    for mm_name in mm_mp_map.keys():
        mp_name = mm_mp_map[mm_name]
        mp_idx = mp_name_idx_map[mp_name]
        mm_idx = mm_name_idx_map[mm_name]
        mp_idx_mm_idx_map[mp_idx] = mm_idx

    # for hips move var
    _, model_right_up_leg = find_model_json(mixamo_bindingpose_json["node"], Mixamo.RightUpLeg.name)
    __, model_right_leg = find_model_json(mixamo_bindingpose_json["node"], Mixamo.RightLeg.name)

    model_scale = 100
    if _ != False:
        model_scale = get_3d_len(model_right_up_leg["position"])*2.0
    model_scale2 =1.0
    if __ !=False:
        model_scale2 = get_3d_len(model_right_leg["position"])
    hip_move_list = []
    origin = None
    factor = 0.0
    factor_list = []
    width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
    height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)

    frame_num = -1
    matplotlib.pyplot.ion()
    matplotlib.pyplot.close()
    fig = None
    if mp_manager.is_show_result:
        fig = matplotlib.pyplot.figure()
        matplotlib.pyplot.show()

    # init mediapipe
    try:
        max_frame_num = mp_manager.max_frame_num
        is_show_result = mp_manager.is_show_result
        min_visibility = mp_manager.min_visibility
        is_hips_move = mp_manager.is_hips_move
        while cap.isOpened():

            success, cap_image = cap.read()
            frame_num += 1
            if not success or max_frame_num < frame_num:
                break
            height1, width1, _ = cap_image.shape
            cap_image = cv2.resize(
                cap_image, (int(width1 * (640 / height1)), 640))
            height2, width2, _ = cap_image.shape
            height = height2
            width = width2
            cap_image, glm_list, visibility_list, hip2d_left, hip2d_right, leg2d = detect_pose_to_glm_pose(
                mp_manager, cap_image, mp_idx_mm_idx_map)
            if glm_list[0] != None:
                bones_json = {
                    "time": math.floor(frame_num*time_factor),
                    "bones": []
                }
                mixamo_bindingpose_root_node.normalize(glm_list)
                mixamo_bindingpose_root_node.calc_animation(glm_list)
                mixamo_bindingpose_root_node.tmp_to_json(bones_json, visibility_list, min_visibility)
                anim_result_json["frames"].append(bones_json)
                if is_show_result:
                    rg = []
                    rv = [None] * len(glm_list)
                    mixamo_bindingpose_root_node.get_vec_and_group_list(
                        rv, rg, is_apply_animation_transform=True)
                    matplotlib.pyplot.clf()
                    # draw_list2(fig, rv, rg)
                    draw_list3(fig,rv, glm_list, rg)
                if is_hips_move:
                    hip2d_left.x *= width
                    hip2d_left.y *= height
                    hip2d_left.z *= width
                    hip2d_right.x *= width
                    hip2d_right.y *= height
                    hip2d_right.z *= width
                    leg2d.x *=width
                    leg2d.y *=height
                    leg2d.z *=width

                    if origin == None:
                        origin = avg_vec3(hip2d_left, hip2d_right)
                    else:
                        hips2d_scale = glm.distance(hip2d_left, hip2d_right)
                        leg2d_scale = glm.distance(leg2d, hip2d_right)
                        factor_list.append(model_scale2/leg2d_scale)
                        factor = max(factor, model_scale/hips2d_scale)
                        hip_move_list.append([len(anim_result_json["frames"]) -1, avg_vec3(hip2d_left, hip2d_right)])

            cv2.imshow('MediaPipe pose', cap_image)
            key = cv2.waitKey(5)
            if key & 0xFF == 27:
                break
        factor_list.sort()
        factor_list_avg = sum(factor_list)/len(factor_list)
        factor_list_avg = max(factor_list_avg, factor_list[int(len(factor_list)*0.8)])
        factor = max(factor,factor_list_avg)
        if mp_manager.factor != 0.0:
           factor = mp_manager.factor
        print("factor", factor)
        for hips_bone in hip_move_list:
            set_hips_position(find_bones(anim_result_json["frames"][hips_bone[0]]["bones"], Mixamo.Hips.name)["position"],
                              origin, 
                              hips_bone[1], 
                              factor)
        if anim_result_json["frames"][0]["time"] != 0.0:
            tmp_json = copy.deepcopy(anim_result_json["frames"][0])
            tmp_json["time"] = 0.0
            anim_result_json["frames"].append(tmp_json)
        
        cap.release()
        # matplotlib.pyplot.close(fig)
        cv2.destroyAllWindows()

    except Exception as e:
        print(e)
        # matplotlib.pyplot.close(fig)
        if cap.isOpened():
            cap.release()
            cv2.destroyAllWindows()


def detect_pose_to_glm_pose(mp_manager, image, mp_idx_mm_idx_map):
    # Create a copy of the input image.
    output_image = image.copy()

    image.flags.writeable = False

    # Convert the image from BGR into RGB format.
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Perform the Pose Detection.
    results = mp_manager.get_pose().process(image_rgb)

    image.flags.writeable = True

    # Initialize a list to store the detected landmarks.
    glm_list = [None]*26
    visibility_list = [None]*26
    hip2d_left, hip2d_right = glm.vec3(0.0, 0.0, 0.0), glm.vec3(0.0, 0.0, 0.0)

    if results.pose_world_landmarks:
        landmark = results.pose_world_landmarks.landmark

        glm_list[Mixamo.Hips] = avg_vec3(
            landmark[mp_manager.mp_pose.PoseLandmark.LEFT_HIP], landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_HIP])
        visibility_list[Mixamo.Hips] = (landmark[mp_manager.mp_pose.PoseLandmark.LEFT_HIP].visibility +
                                        landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_HIP].visibility)*0.5
        glm_list[Mixamo.Neck] = avg_vec3(
            landmark[mp_manager.mp_pose.PoseLandmark.LEFT_SHOULDER], landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_SHOULDER])
        visibility_list[Mixamo.Neck] = (landmark[mp_manager.mp_pose.PoseLandmark.LEFT_SHOULDER].visibility +
                                        landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_SHOULDER].visibility)*0.5
        glm_list[Mixamo.Spine1] = avg_vec3(
            glm_list[Mixamo.Hips], glm_list[Mixamo.Neck])
        visibility_list[Mixamo.Spine1] = (
            visibility_list[Mixamo.Hips] + visibility_list[Mixamo.Neck])*0.5
        glm_list[Mixamo.Spine] = avg_vec3(
            glm_list[Mixamo.Hips], glm_list[Mixamo.Spine1])
        visibility_list[Mixamo.Spine] = (
            visibility_list[Mixamo.Hips] + visibility_list[Mixamo.Spine1])*0.5
        glm_list[Mixamo.Spine2] = avg_vec3(
            glm_list[Mixamo.Spine1], glm_list[Mixamo.Neck])
        visibility_list[Mixamo.Spine2] = (
            visibility_list[Mixamo.Spine1] + visibility_list[Mixamo.Neck])*0.5
        glm_list[Mixamo.Head] = avg_vec3(
            landmark[mp_manager.mp_pose.PoseLandmark.LEFT_EAR], landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_EAR])
        visibility_list[Mixamo.Head] = (landmark[mp_manager.mp_pose.PoseLandmark.LEFT_EAR].visibility +
                                        landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_EAR].visibility)*0.5

        glm_list[Mixamo.Spine].y *= -1
        glm_list[Mixamo.Neck].y *= -1
        glm_list[Mixamo.Spine1].y *= -1
        glm_list[Mixamo.Spine2].y *= -1
        glm_list[Mixamo.Head].y *= -1

        glm_list[Mixamo.Neck].z *= -1
        glm_list[Mixamo.Spine].z *= -1
        glm_list[Mixamo.Spine1].z *= -1
        glm_list[Mixamo.Spine2].z *= -1
        glm_list[Mixamo.Head].z *= -1
        for mp_idx in mp_idx_mm_idx_map.keys():
            mm_idx = mp_idx_mm_idx_map[mp_idx]
            glm_list[mm_idx] = glm.vec3(
                landmark[mp_idx].x, -landmark[mp_idx].y, -landmark[mp_idx].z)
            visibility_list[mm_idx] = landmark[mp_idx].visibility

    # 2d landmarks
    leg2d = None
    if results.pose_landmarks:
        landmark = results.pose_landmarks.landmark
        hip2d_left.x = landmark[mp_manager.mp_pose.PoseLandmark.LEFT_HIP].x
        hip2d_left.y = landmark[mp_manager.mp_pose.PoseLandmark.LEFT_HIP].y
        hip2d_left.z = landmark[mp_manager.mp_pose.PoseLandmark.LEFT_HIP].z
        hip2d_right = glm.vec3(landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_HIP].x,
                               landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_HIP].y, landmark[mp_manager.mp_pose.PoseLandmark.RIGHT_HIP].z)
        leg2d = glm.vec3(landmark[26].x, landmark[26].y, landmark[26].z)
        

    mp_manager.mp_drawing.draw_landmarks(image=output_image, landmark_list=results.pose_landmarks,
                                         connections=mp_manager.mp_pose.POSE_CONNECTIONS, landmark_drawing_spec=mp_manager.mp_drawing_styles.get_default_pose_landmarks_style())

    return output_image, glm_list, visibility_list, hip2d_left, hip2d_right, leg2d


def avg_vec3(v1, v2):
    v3 = glm.vec3((v1.x + v2.x) * 0.5,
                  (v1.y + v2.y) * 0.5,
                  (v1.z + v2.z) * 0.5)
    return v3


def set_hips_position(hips_bone_json, origin_hips, current_hips, factor):
    x = (current_hips.x - origin_hips.x) * factor
    y = (current_hips.y - origin_hips.y) * factor
    z = (current_hips.z - origin_hips.z) * factor
    hips_bone_json["x"] = x 
    hips_bone_json["y"] = -y
    hips_bone_json["z"] = z