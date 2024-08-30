Sdi2000292 Fotopoulos Dimitrios
Sdi2000290 Kammas Zisis

H class CLustering περιέχει σαν private μεταβλητές το Κ που ειναι ο αριθμός των
Cluster που θέλουμε, μεταβλητές αρχικοποίησης του LSH και του Cube(οπως Lsh_K,Lsh_L
Cube_K,Cube_M,Cube_Probes), το vectors που εκει θα αποθηκευτεί το dataset, centroids
όπου αποθηκεύονται τα κέντρα απο το κάθε Cluster, τον πίνακα ClusterPerVector που
για κάθε διάνυσμα της βάσης έχουμε το Cluster που ανοίκει. Εναν δείκτη σε LSH ή Cube instance
, έναν πίνακα ClusterPerVector όπου για κάθε image αποθηκεύεται ένας αριθμός που αντιπροσωπεύει το κέντρο στο οποίο έχει 
αντιστοιχιστεί, και δύο private συναρτήσεις readConfig που διαβάζει το 
cluster.conf για την αρχικοποιηση των μεταβλητών και silhouetteCiefficient που υλοποιεί για κάθε σημείο την συνάρτηση της
silhouette, επίσης η centroidInitiation που αρχικοποιεί τα κεντροειδή με τον αλγό-
ριθμο kmeans++.

Ως public συναρτήσεις έχουμε τον constructor που αρχικοποιεί τα παραπάνω με  
βάση το αρχείο "cluster.conf" επίσης γίνεται το Initiation των κεντροειδών, και
με αντιστοιχες συνθηκες οι αρχικοποιήσεις των LSH, και Cube αν χρειάζεται.
Των lloydsAlgorithm() που αναθέτει τα διανύσματα στα κεντροειδή και επαναπροσδιο-
ρίζει κάθε κεντροειδή ανάλογα με την καινούργια κατάσταση(Brute Force Method),
τοποθετέι κάθε point σε κάθε κεντροειδή.

Η averageSilhouette συνάρτηση χρησιμοποιεί την private silhouetteCiefficient ωστε 
να βρει το average s(i) (Oπου s(i) η συνάρτηση silhouette για κάθε vector και i το
κάθε vector) 

