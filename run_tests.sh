#!/bin/bash

mkdir -p Results/Logs
mkdir -p Results/SampleInputs
mkdir -p Reports

for d in "MNIST" "FashionMNIST" "SVHN" "TaxiNet" "Udacity"
do
	echo "Running tests for "$d
	for r in {1..10} #repeat the experiment n times
	do
		echo "Running iteration "$r
		filename="Results/Logs/"$d"_results_"$r".txt"
		echo -e "Running experiments for "$d"...\n" > $filename
		echo "Running experiments for "$d
		for i in "Level1" "Level2" "Level3" "Level4" "Level5"
		do
			echo "Generating latent samples for "$i >> $filename
			echo "Generating latent samples for "$i
			echo "python gen_latent_samples.py --dataset "$d " --density_range "$i "--index "$r >> $filename
			python gen_latent_samples.py --dataset $d --density_range $i --index $r &>> $filename
			echo -e "\nGenerating test set for "$i >> $filename
			echo "Generating test set for "$i
			echo "python gen_test_inputs.py --dataset "$d " --density_range "$i "--index "$r >> $filename
			python gen_test_inputs.py --dataset $d --density_range $i --index $r >> $filename
			cd DNN
			echo "Meauring faults for "$i
			echo -e "\nMeauring faults for "$i >> "../"$filename
			echo "python faults.py --dataset "$d " --density_range "$i "--index "$r >> "../"$filename
			python faults.py --dataset $d --density_range $i --index $r >> "../"$filename
			cd ..
			echo -e "\n\n" >> $filename
		done
	done
done

echo "Generating report..."
python rq3_report.py MNIST FashionMNIST SVHN TaxiNet Udacity 10
