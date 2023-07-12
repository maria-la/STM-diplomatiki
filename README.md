# STM-diplomatiki

Το κυριο branch που δουλευει εως και fft ειναι το main του φακελου 01_adc_multiple_inputs

το 01_adc_multiple_inputs2 αγνοειται ειναι λαθος και καποια στιημη θα διαγραφει

στο fastICA_tests μετα απο copy του 01_adc_multiple_inputs main προσθετω τον κωδικα για fastICA 


τωρα branches:

- fastICA_noDynamAlloc(FAIL): στο 01_adc_multiple_inputs main προστιθεται μια βιβλιοθηκη fastICA και προσπαθω να εξαλειψω dynamic allocation 
- fastICAcoder(FAIL): στο 01_adc_multiple_inputs main προστιθεται μια βιβλιοθηκη fastICA που εχει προκυψει απο matlab coder για stm board-> fail γιατι εχει
                       dynamic allocation και χωρις αυτο δεν κανει generate γιατι δε χωραει για fft 128 μηκος
- fftTrial(OUTDATED): παλιο branch για οταν δν δουλευε ο fft
- ilrma(ON HOLD): προσπαθησα να υλοποιησω τον ilrma με βιβλιοθηκη cmsis εχω κολλησει σε ενα σημειο με 3διαστατους πινακες- λογικα βγαινει αλλα παραειναι περιπλοκο
- ilrma_coder(FAIL): ακριβως το ιδιο με fastICAcoder αλλα για ilrma
  
-- fastICA_test_mycode (ΣΕ ΑΥΤΟ ΔΟΥΛΕΥΩ): επεξεργαζομαι το fastICA_tests για να φτιαξω τον fastICA με τη βοηθεια arm_math.h -- πρεπει να ελεγξω τη λειτουργικοτητα με συγκρισεις με Matlab

