import csv
import math
import argparse
from collections import Counter, defaultdict

class TreeNode:
    def __init__(self, attribute=None, is_leaf=False, label=None):
        self.attribute = attribute      
        self.children = {}
        self.is_leaf = is_leaf
        self.label = label

    def add_child(self, value, node):
        self.children[value] = node


def load_data(path):
    with open(path, 'r', encoding='utf-8') as f:
        reader = csv.reader(f, delimiter='\t')
        header = next(reader)
        data = [dict(zip(header, row)) for row in reader]
    return header, data


def entropy(records, target_attr):
    total = len(records)
    if total == 0:
        return 0
    counts = Counter(r[target_attr] for r in records)
    ent = 0.0
    for count in counts.values():
        p = count / total
        ent -= p * math.log2(p)
    return ent


def split_records(records, attr):
    splits = defaultdict(list)
    for r in records:
        splits[r[attr]].append(r)
    return splits


def gain_ratio(records, attr, target_attr):
    base_ent = entropy(records, target_attr)
    splits = split_records(records, attr)
    total = len(records)
    cond_ent = 0.0
    split_info = 0.0
    for subset in splits.values():
        p = len(subset) / total
        cond_ent += p * entropy(subset, target_attr)
        split_info -= p * math.log2(p) if p > 0 else 0
    info_gain = base_ent - cond_ent
    if split_info == 0:
        return 0
    return info_gain / split_info


def choose_best_attribute(records, attributes, target_attr):
    best_attr = None
    best_gain_ratio = -1
    for attr in attributes:
        gr = gain_ratio(records, attr, target_attr)
        if gr > best_gain_ratio:
            best_gain_ratio = gr
            best_attr = attr
    return best_attr


def majority_label(records, target_attr):
    counts = Counter(r[target_attr] for r in records)
    return counts.most_common(1)[0][0]


def build_tree(records, attributes, target_attr):
    labels = [r[target_attr] for r in records]
    if len(set(labels)) == 1:
        return TreeNode(is_leaf=True, label=labels[0])

    if not attributes:
        return TreeNode(is_leaf=True, label=majority_label(records, target_attr))

    best_attr = choose_best_attribute(records, attributes, target_attr)
    tree = TreeNode(attribute=best_attr)
    splits = split_records(records, best_attr)

    for attr_value, subset in splits.items():
        if not subset:
            leaf = TreeNode(is_leaf=True, label=majority_label(records, target_attr))
            tree.add_child(attr_value, leaf)
        else:
            remaining_attrs = [a for a in attributes if a != best_attr]
            subtree = build_tree(subset, remaining_attrs, target_attr)
            tree.add_child(attr_value, subtree)
    return tree


def classify(record, tree):

    if tree.is_leaf:
        return tree.label
    attr_value = record.get(tree.attribute)
    child = tree.children.get(attr_value)
    if child:
        return classify(record, child)
    else:
        return majority_label([record], tree.attribute)


def write_output(test_header, test_records, tree, output_path, target_attr):
    with open(output_path, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t')
        out_header = test_header + [target_attr]
        writer.writerow(out_header)
        for rec in test_records:
            label = classify(rec, tree)
            row = [rec.get(col, '') for col in test_header] + [label]
            writer.writerow(row)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('train')
    parser.add_argument('test')
    parser.add_argument('output')
    args = parser.parse_args()

    train_header, train_data = load_data(args.train)
    test_header, test_data = load_data(args.test)

    target_attr = train_header[-1]
    feature_attrs = [a for a in train_header if a != target_attr]

    tree = build_tree(train_data, feature_attrs, target_attr)

    write_output(test_header, test_data, tree, args.output, target_attr)

if __name__ == '__main__':
    main()
