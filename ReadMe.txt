========================================================================
    CONSOLE APPLICATION : AS4_224101015 Project Overview
========================================================================

Project is divided into two parts 
1. Training 
2. Testing 

In training phase 10 recorded file of each vowel is taken 
for each of the vowel and for each of the recorder signal following steps are executed 

    1.Dc shift is computed taking 1st 100values and that is deducted from all the amplitudes. 
    2.The file is normalized.
    3.5 steady frame is computed i.e. the frame with maximum energy and 2frames before and after the maximum energy frame. 
    4.for each frame using durbin's algorithm ai value are computed (following the auto correlation method )
    5.from ai values for each frame cepstral values are also computed 
    6.Raised sine window function is applied on the ci values 
    7.ci values are computed for each steady frames and later average of cis are taken to find the ci values of that signal 
Now we got ci values of each vowel and each recorded speech samples 

Now the training is over 

Testing Phase ---

1. Recorded speech is taken 
2. Normalisation and dc shift reduduction is done 
3. Now 5 steady frames are taken out and ci values of each steady frame is computed 
4. Now for each speech signal we have 5sets of ci values (ci values are computed following the same path that was
   followed in training phase)
5. Average of 5 ci vectors is computed 
6. now we got one ci vector of that test speech signal 


Now prediction part comes 

For given test speech,we have ci values these are compared with the ci values found in training phase, 
the vowel from training phase which has the most less Thokura's distance that is taken and that vowel is predicted as the current test vowel sample 


Tools used-
Visual Studio 2010 to code the solution in C language 
cooledit 2000 to record the voice speech.
    

Prediction accuracy--
a prediction Accuracy is 100.000000%
e prediction Accuracy is 100.000000%
i prediction Accuracy is 100.000000%
o prediction Accuracy is 100.000000%
u prediction Accuracy is 90.000000%
