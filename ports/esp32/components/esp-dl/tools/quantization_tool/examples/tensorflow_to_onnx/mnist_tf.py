import tensorflow as tf
from tensorflow import keras

import tf2onnx


model = tf.keras.models.Sequential(
        [tf.keras.layers.Flatten(input_shape=(28,28)),
         tf.keras.layers.Dense(128,  activation = 'relu'),
         tf.keras.layers.Dense(64, activation='relu'),
         tf.keras.layers.Dense(10, activation='softmax')
         ]
    )

def simple_mnist_model():
    mnist = keras.datasets.mnist
    (train_images, train_labels), (test_images, test_labels) = mnist.load_data()

    train_images = train_images / 255.0
    test_images = test_images / 255.0

    model.compile(optimizer='adam',
                      loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                      metrics=['accuracy'])


    checkpoint_path = "simple_mnist_model_cp-{epoch:02d}.ckpt"

    cp_callback = tf.keras.callbacks.ModelCheckpoint(filepath=checkpoint_path,
                                                     save_weights_only=True,
                                                     verbose=1,
                                                     period=5)

    model.fit(train_images,
              train_labels,
              epochs=10,
              validation_data=(test_images, test_labels),
              callbacks=[cp_callback])


if __name__ == "__main__":
    # train the model
    # simple_mnist_model()

    model.load_weights('simple_mnist_model_cp-10.ckpt')
    model.summary()

    # export model to onnx format
    spec = (tf.TensorSpec((None, 28, 28), tf.float32, name="input"),)
    output_path = "mnist_model_tf.onnx"
    model_proto, _ = tf2onnx.convert.from_keras(model, input_signature=spec, opset=13, output_path=output_path)

    checker.check_graph(model_proto.graph)