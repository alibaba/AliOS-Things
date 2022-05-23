# Espressif Speech Wake Word Customization Process [[中文]](./乐鑫语音唤醒词定制流程.md)

#### Offline Wake Word Customization

Espressif provides users with the **Off-line Wake Word Customization** service, which allows users to use both publicly available Wake Words (such as "Hi Lexin", "ni hao xiao xin", "ni hao xiao zhi" and "Hi Jeson") and customized Wake Words. 

 1. If you want to use publicly available Wake Words for commercial use, 
	- please check the Wake Words provided in ADF/ASR Demos;
	- We will continue to provide more and more Wake Words that are free for commercial use.

 2. If you want to use your own wake words, we can also provide the **Off-line Wake Word Customization** service.
	- If you are able to provide a training corpus meeting the requirements described in the following **Requirements on Corpus**. 
		- We need two to three weeks for training and optimization.
		- Service fee will be charged by Espressif in this case.
		
	- Otherwise
		- We will provide the training corpus (all your corpus won't be comprised and shared)
		- We need two to three weeks for training and optimization.
		- Service fee will be charged by Espressif in this case (Fee incurred from collecting the training corpus will be charged separately).
	
	- For details on the fee and time required for customization, please email us at [sales@espressif.com](sales@espressif.com).
		- We will agree on a reasonable plan based on how many wake words for customization and how large is your scale of product production.
		 
 3. About Espressif Wake Word Model
 	- Now, a single wake word model can recognize up to five Wake Words
 	- Normally, each Wake Word contains three to six syllables, such as "Hi Le xin" (3 syllables), “Alexa” (3 syllables), "小爱同学" (4 syllables).
	- Several wake words can be used in combination based on your actual requirement.

#### Requirements on Corpus Texts

You can provide us your training corpus by preparing it yourself or purchasing one from a third party service provider. However, please make sure your corpus meets the following requirements.  

- Audio File Format
	- Sample rate: 16 KHz
	- Encoding method: 16-bit signed int
	- Channel type: mono
	- File format: wav	

- Sampling
	- Sample size: no less than 500 people, among which,
		- The number of males and females should be similar;
		- The number of people in different age-group should be similar;
		- The number of children should be larger than 100 (If children are one of your target users). 
	- Environment: 
		- It's advise to collect your sample with a Hi-Fi microphone in a professional audio room, with an ambient noise lower than 40 dB.
		- Each participant should  
			- Position himself/herself at a distance of one meter from the microphone, and repeat the Wake Word for 15 times (5 times fast, 10 times normal);
			- Position himself/herself at a distance of three meters from the microphone, and repeat the Wake Word for 15 times (5 times fast, 10 times normal);
	- The naming of sample file should reflect the sex, age, and speech speed of the sample himself/herself. An example for naming your sample file is `female_age_fast_id.wav`. Or you can provide a separate form to record these information.

#### Hardware Design and Test

1. The performance of wake word detection is heavily impacted by the hardware design and cavity structure. Therefore, please go through the following requirements on hardware design.
	
	- Hardware design: We provide reference design files for smart speakers, including schematic diagrams and PCB designs. Please refer to these files when designing your own speaker. It's advised that you send your designs to Espressif for review to avoid some most common design issues.

	- Cavity structure: We don't provide reference designs for cavity structures. Therefore, it's advised to involve acoustic professionals during the design and take reference form other mainstream speakers in the market, such as TmallGenie（天猫精灵）, Baidu speaker（小度音箱）and Google speaker（谷歌音箱）.		
				
2. You can evaluate the performance of your design by performing the following tests. Note that all the tests below are designed to be performed in an audio room. Please make adjustment according to your actual situation.

	- Record test to evaluate the gain and distortion for MIC and codec.
  		 - Play audio samples (90 dB, 0.1 meter away from the MIC), and make sure the recording sample is not unsaturated by adjusting the gain of MIC. 
  		 - Play frequency sweep file (0~20 KHz), and record it using a sample rate of 16 KHz. No prominent aliasing should be observed. 
  		 - Use the publicly released speech recognize API provided on the cloud to recognize 100 audio samples. The recognition rate should meet certain standard.

	- Playing test to verify the distortion of the PA and speaker by measuring: 
  		- PA power @1% THD.

	- Test the performance of DSP, and verify if the DSP parameters are configured correctly, meanwhile minimizing the non-linear distortion in the DSP arithmetic.
		- Test the performance of the **Noise Suppression** algorithm
		- Test the performance of the **Acoustic Echo Cancellation** algorithm
		- Test the performance of the **Speech Enhancement** algorithm

3. After your hardware design, it's advised to **send** 1 or 2 pieces of your hardware, so we can optimize its performance for wake word detection on a whole product level.





