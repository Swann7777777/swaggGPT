# swaggGPT

## Description

**SwaggGPT** is a special coding project.  
The objective is to create a fully working *LLM* from scratch.  
*"from scratch"* means that external libraries such as mlpack (or tensor for python) are not allowed.  
This project is made for learning purposes so do not take the code as an example.  

## Features

- **Tokenizer**  
A tokenizer is a component of the NLP pipeline.  
It creates a list of tokens that can be words of pieces of words.

- **Embedding model**  
An embedding model is also a component of the NLP pipeline.  
It creates an embed for each token.  
These embed serve as "meaning" of the words for the Transformer.

- **Transformer**  
A transformer is an artificial neural network architecture.  
At the end of the project, it will be able to take a text input and generate an answer.  

## Install

```
# Clone the repository
git clone https://github.com/Swann7777777/swaggGPT

# Change the working directory to swaggGPT
cd swaggGPT

# Create a build direcory
mkdir build && cd build

# Build the project
cmake ..

# Generate the executable
make

# Run the project
./main