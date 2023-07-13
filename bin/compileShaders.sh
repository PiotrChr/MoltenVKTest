#!/bin/bash

currentDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
resourcesDir="${currentDir}/../Resources"
shaderDir="${resourcesDir}/shaders/"
compiledShadersDir="${resourcesDir}/compiledShaders/"

GLSLC="${currentDir}/glslc"

if [ ! -f "${GLSLC}" ]; then
  echo "Error GLSLC: ${GLSLC} does not exist"
  exit 1
fi

if [ ! -d "${shaderDir}" ]; then
  echo "Error shaderDir: ${shaderDir} does not exist"
  exit 1
fi

if [ ! -d "${compiledShadersDir}" ]; then
  echo "Error compiledShadersDir: ${compiledShadersDir} does not exist"

  echo "Creating ${compiledShadersDir}"
  mkdir "${compiledShadersDir}"
fi

for file in "${shaderDir}"*.vert; do
  echo "Compiling ${file}"
  "${GLSLC}" -fshader-stage=vert -o "${compiledShadersDir}/$(basename "${file%.*}").vert.spv" "${file}"
done

for file in "${shaderDir}"*.frag; do
  echo "Compiling ${file}"
  "${GLSLC}" -fshader-stage=frag -o "${compiledShadersDir}/$(basename "${file%.*}").frag.spv" "${file}"
done