#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    bodyCalculator.py
@Description:    bmi身体数据计算
@Author     :    aoyi
@version    :    1.0
'''

MALE = 1
FEMALE = 2
ACTIVITY_SEDENTARY = 1
ACTIVITY_MILD_INTENSITY = 2
ACTIVITY_MIDDLE_INTENSITY = 3
ACTIVITY_HIGH_INTENSITY = 4
ACTIVITY_HEAVY_PHYSICAL_LABOR = 5
BMI_UNDERWEIGHT = 18.5
BMI_NORMAL = 24
BMI_OVERWEIGHT = 27

class BodyCalculator :
    def __init__(self) :
        self.height = 1#避免bmi计算时，除0错误
        self.weight = 0
        self.age = 0
        self.gender = 0
        self.activity_intensity = 0
        self.bmi = 0
        self.bmr = 0
        self.tdee = 0

    def set_height(self, height):
        if height > 0:
            self.height = height / 100

    def set_weight(self, weight):
        if weight > 0:
            self.weight = weight

    def set_age(self, age):
        if age > 0:
            self.age = age

    def set_gender(self, gender):
        if gender > 0 and gender < 3:
            self.gender = gender

    def set_activity_intensity(self, activity_intensity):
        if activity_intensity >= ACTIVITY_SEDENTARY  and activity_intensity <= ACTIVITY_HEAVY_PHYSICAL_LABOR:
            self.activity_intensity = activity_intensity

    def calc_body_data(self):
        if ACTIVITY_SEDENTARY == self.activity_intensity:
            multi_factor = 1.2
        elif ACTIVITY_MILD_INTENSITY == self.activity_intensity:
            multi_factor = 1.375
        elif ACTIVITY_MIDDLE_INTENSITY == self.activity_intensity:
            multi_factor = 1.55
        elif ACTIVITY_HIGH_INTENSITY == self.activity_intensity:
            multi_factor = 1.725
        elif ACTIVITY_HEAVY_PHYSICAL_LABOR == self.activity_intensity:
            multi_factor = 1.9
        else:
            multi_factor = 1

        if MALE == self.gender:
            self.bmr = (10 * self.weight) + (6.25 * self.height * 100) - (5 * self.age) + 5
        elif FEMALE == self.gender:
            self.bmr = (10 * self.weight) + (6.25 * self.height * 100) - (5 * self.age) - 161
        else:
            print('gender illegeal')

        self.bmr = round(self.bmr, 2)
        self.tdee = round(self.bmr * multi_factor, 2)
        self.bmi = round(self.weight / (self.height * self.height), 2)
        print('height:' + str(self.height) + ' weight:' + str(self.weight) + ' bmi :' + str(self.bmi) + ' bmr :' + str(self.bmr)+ ' tdee :' + str(self.tdee))

    def set_body_data(self, data):
        if 'Height' in data.keys():
            self.set_height(data['Height'])

        if 'Weight' in data.keys():
            self.set_weight(data['Weight'])

        if 'Age' in data.keys():
            self.set_age(data['Age'])

        if 'Gender' in data.keys():
            self.set_gender(data['Gender'])

        if 'Activity_intensity' in data.keys():
            self.set_activity_intensity(data['Activity_intensity'])

        self.calc_body_data()

    def generate_diet_suggestion(self, total_food_calorie):
        if 0 == self.bmi or 0 == self.bmr or 0 == self.tdee:
            suggest_str = '信息不全，请补充'
        else:
            if self.bmi > 0 and self.bmi < BMI_UNDERWEIGHT :
                suggest_str = 'bmi = ' + str(self.bmi) + '，体重过轻，请增加每日饮食摄入。\n建议每日摄入大于' + str(self.tdee + 500) + '卡路里，今日已摄入'+ str(total_food_calorie) + '卡路里'
            elif self.bmi > BMI_UNDERWEIGHT and self.bmi < BMI_NORMAL:
                suggest_str = 'bmi = ' + str(self.bmi) + ', 正常，请保持，建议每日摄入' + str(self.tdee - 500) + '卡路里。\n今日已摄入'+ str(total_food_calorie) + '卡路里'
            elif self.bmi > BMI_NORMAL and self.bmi < BMI_OVERWEIGHT:
                suggest_str = 'bmi = ' + str(self.bmi) + ', 处于超重范围，需要减少摄入。\n建议每日摄入小于' + str(self.tdee - 500) + '卡路里。今日已摄入'+ str(total_food_calorie) + '卡路里'
            elif self.bmi > BMI_OVERWEIGHT:
                suggest_str = 'bmi = ' + str(self.bmi) + ', 处于肥胖范围，需要减少摄入。\n建议每日摄入小于' + str(self.tdee - 1000) + '卡路里。今日已摄入'+ str(total_food_calorie) + '卡路里'
            else:
                suggest_str = '信息不全，请补充'

        print('生成的饮食建议' + suggest_str)
        return suggest_str

    def get_body_bmi(self):
        return self.bmi
