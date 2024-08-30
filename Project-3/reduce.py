# call: $python reduce.py -en <encoder_file> -od <output_dataset_file> -oq <output_query_file>
# Takes as input a dataset and a queryset and reduces their dimensionality using the encoder part of the autoencoder

import argparse
import tensorflow as tf
from tensorflow.keras.models import load_model, Model
from tensorflow.keras.datasets import mnist  # take the mnist dataset directly from keras 
import struct

def write_to_binary(data, filename, dimensions):
    magic_number = 666 
    imageNum = data.shape[0]
    rows = 1 # each image is now a single row
    cols = dimensions
    
    with open(filename, 'wb') as file:
        file.write(struct.pack('>I', magic_number)) # '>I' ensures it is written in big-endian
        file.write(struct.pack('>I', imageNum))
        file.write(struct.pack('>I', rows))
        file.write(struct.pack('>I', cols))
        
        for image in data: 
            file.write(image.tobytes()) # the image is already flattened by the model, so it can be written directly to the file



# encoderName = 'best_model.h5' # put here the proper name of the encoder you want to use

# parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('-en', '--encoder_name', help='name of the encoder to use', required=False)
parser.add_argument('-od', '--output_dataset', help='output dataset file', required=False, default='dataset_reduced.bin')
parser.add_argument('-oq', '--output_query', help='output query file', required=False, default='queryset_reduced.bin')
args = parser.parse_args()
# save the arguments in variables
if args.encoder_name:
    encoderName = args.encoder_name
else:
    encoderName = input("\nPlease enter the name of the encoder you want to use: ")
output_dataset = args.output_dataset
output_query = args.output_query

# load the dataset and queryset from mnist
(dataset, _), (queryset, _) = mnist.load_data()
dataset_normalized, queryset_normalized = dataset / 255.0, queryset / 255.0


# print the arguments
print(f"Dataset shape: (num_images, rows, cols) = {dataset.shape}")
print(f"Queryset shape: (num_images, rows, cols) = {queryset.shape}")

# load the model
autoencoder = load_model(encoderName)

# keep the encoder part
encoder = Model(inputs=autoencoder.input, outputs=autoencoder.layers[6].output)
print(f"\nLoaded encoder with name '{encoderName}'.")

# do the magic
print("Reducing the dimensionality of the datasets...")
reduced_dataset_normalized = encoder.predict(dataset_normalized)
print(f"Reduced_dataset created. Shape is {reduced_dataset_normalized.shape}")
reduced_queryset_normalized = encoder.predict(queryset_normalized)
print(f"Reduced_queryset created. Shape is {reduced_queryset_normalized.shape}")

# scale back the values to [0, 255] as unsigned 8-bit integers
reduced_dataset = (reduced_dataset_normalized * 255.0).astype('uint8')    
reduced_queryset = (reduced_queryset_normalized * 255.0).astype('uint8')

# write the reduced datasets to binary files
write_to_binary(reduced_dataset, output_dataset, reduced_dataset.shape[1])
write_to_binary(reduced_queryset, output_query, reduced_queryset.shape[1])

print("\nDatasets written to binary files:")
print(f"Output dataset file: {output_dataset}")
print(f"Output queryset file: {output_query}")





