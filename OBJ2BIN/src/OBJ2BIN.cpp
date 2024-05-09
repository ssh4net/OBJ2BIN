/*
 * Copyright (c) 2023 Erium Vladlen.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <tuple>
#include <chrono>
#include "rapidobj/rapidobj.hpp"

void ReportError(const rapidobj::Error& error)
{
    std::cout << error.code.message() << "\n";
    if (!error.line.empty()) {
        std::cout << "On line " << error.line_num << ": \"" << error.line << "\"\n";
    }
}

// Custom hash function for tuples
struct tuple_hash {
    template <class T1, class T2, class T3>
    std::size_t operator () (const std::tuple<T1, T2, T3>& tuple) const {
        auto [x, y, z] = tuple;
        size_t res = 17;
        res = res * 31 + std::hash<T1>()(x);
        res = res * 31 + std::hash<T2>()(y);
        res = res * 31 + std::hash<T3>()(z);
        return res;
    }
};

// Function to write binary data
template<typename T>
void writeBinaryData(const std::string& filename, const std::vector<T>& data) {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
        out.close();
    }
    else {
        std::cerr << "Failed to open " << filename << " for writing.\n";
    }
}

// Function to write ASCII data
template<typename T>
void writeAsciiData(const std::string& filename, const std::vector<T>& data, const int fields) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << std::fixed << std::setprecision(6); // Sets the decimal precision to 6
		int i = 1;
        for (const T& value : data) {
			out << value << ((i++ % fields == 0) ? "\n" : " ");
        }
        out.close();
    }
    else {
        std::cerr << "Failed to open " << filename << " for writing.\n";
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
		std::cerr << "Usage: OBJ2BIN path_to_file.obj [ASCII]\n";
		return 1;
	}

	auto start = std::chrono::high_resolution_clock::now();

    // Load the OBJ file
	rapidobj::Result result = rapidobj::ParseFile(argv[1]);
    if (result.error) {
        ReportError(result.error);
        return EXIT_FAILURE;
    }

	auto end1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed1 = end1 - start;

	std::cout << "OBJ file loaded successfully in " << elapsed1.count() << " seconds.\n";

	std::filesystem::path obj_path = argv[1];
	std::filesystem::path obj_dir = obj_path.parent_path();
	std::filesystem::path obj_name_we = obj_path.stem();
    
	std::string ascii_str = (argc == 3) ? argv[2] : "";

	if (!ascii_str.empty()) {
		std::transform(ascii_str.begin(), ascii_str.end(), ascii_str.begin(),
			[](unsigned char c) { return std::tolower(c); });
	}

	bool is_ascii = (argc == 3 && std::string(ascii_str) == "ascii");

    if (result.shapes.empty()) {
        std::cerr << "No shapes found in OBJ file.\n";
        return 1;
    }

    const auto& mesh = result.shapes[0].mesh;
    const auto& attributes = result.attributes;

    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<uint32_t> indices;

    std::unordered_map<std::tuple<int, int, int>, uint32_t, tuple_hash> uniqueVertices;

    uint32_t index = 0;
    for (const auto& ind : mesh.indices) {
        std::tuple<int, int, int> key(ind.position_index, ind.texcoord_index, ind.normal_index);
        if (uniqueVertices.find(key) == uniqueVertices.end()) {
            uniqueVertices[key] = index++;

            // Append vertex coordinates
            vertices.push_back(attributes.positions[3 * ind.position_index]);
            vertices.push_back(attributes.positions[3 * ind.position_index + 1]);
            vertices.push_back(attributes.positions[3 * ind.position_index + 2]);

            // Append texture coordinates
            uvs.push_back(attributes.texcoords[2 * ind.texcoord_index]);
            uvs.push_back(attributes.texcoords[2 * ind.texcoord_index + 1]);

            // Append normals
            normals.push_back(attributes.normals[3 * ind.normal_index]);
            normals.push_back(attributes.normals[3 * ind.normal_index + 1]);
            normals.push_back(attributes.normals[3 * ind.normal_index + 2]);
        }
        indices.push_back(uniqueVertices[key]);
    }

	auto end2 = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed2 = end2 - end1;
	std::cout << "Data processed successfully in " << elapsed2.count() << " seconds.\n";

	if (is_ascii) {
		std::cout << "Exporting data to ASCII files...\n";
		// Write data to ASCII files
		writeAsciiData((obj_dir / obj_name_we += "_vert.txt").string(), vertices, 3);
		writeAsciiData((obj_dir / obj_name_we += "_uv.txt").string(), uvs, 2);
		writeAsciiData((obj_dir / obj_name_we += "_norm.txt").string(), normals, 3);
		writeAsciiData((obj_dir / obj_name_we += "_idxs.txt").string(), indices, 3);
	}
	else {
		std::cout << "Exporting data to binary files...\n";
		// Write data to binary files
		writeBinaryData((obj_dir / obj_name_we += "_vert.bin").string(), vertices);
		writeBinaryData((obj_dir / obj_name_we += "_uv.bin").string(), uvs);
		writeBinaryData((obj_dir / obj_name_we += "_norm.bin").string(), normals);
		writeBinaryData((obj_dir / obj_name_we += "_idxs.bin").string(), indices);
	}

	auto end3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed3 = end3 - end2;

	std::cout << "Data exported successfully in " << elapsed3.count() << " seconds.\n";
    return 0;
}