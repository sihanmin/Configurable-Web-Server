let uploaded = false

let handleGoDefault = () => {
  location.href = '/game/default'
}

let handleGoCustomized = () => {
  location.href = `/game/customize/${Cookies.get('customize_id')}`
}

// const toBase64 = file => new Promise((resolve, reject) => {
//   const reader = new FileReader();
//   reader.readAsDataURL(file);
//   reader.onload = re => {
//     let image = new Image()
//     image.onload = () => {
//       let canvas = document.createElement('canvas')
//       image.width = 40
//       image.height = 40
//       canvas.width = 40
//       canvas.height = 40
//       canvas.getContext('2d').drawImage(image, 0, 0, 40, 40)
//       resizedImage = canvas.toDataURL()
//     }
//     image.src = re.target.result
//     return resolve(reader.result)
//   }
//   reader.onerror = error => reject(error);
// })


let handleSubmitImages = async () => {
  document.getElementById('state').innerHTML = 'Processing...'
  document.getElementById('submit').disabled = true
  //  let background = document.getElementById('background').files[0]
  let runner = document.getElementById('runner').files[0]

  const reader = new FileReader();
  reader.onload = re => {
    let image = new Image()
    image.onload = async () => {
      let canvas = document.createElement('canvas')
      let square, stretched

      image.width = 40
      image.height = 40
      canvas.width = 40
      canvas.height = 40
      canvas.getContext('2d').drawImage(image, 0, 0, 40, 40)
      square = canvas.toDataURL()

      image.width = 48
      image.height = 20
      canvas.width = 48
      canvas.height = 20
      canvas.getContext('2d').drawImage(image, 0, 0, 48, 20)
      stretched = canvas.toDataURL()

      let payload = JSON.stringify({ square, stretched })

      try {
        //if (background === undefined) console.log('Missing background image')
        if (runner === undefined) console.log('Missing runner image')

        console.log('Photo uploading')
        let res = await fetch('/game/customize', { method: 'POST', body: payload })
        console.log(res.headers.get('Customize-Id'))

        Cookies.set('customize_id', res.headers.get('Customize-Id'))
        document.getElementById('submit').innerHTML = 'Go'
        uploaded = true

      } catch (e) {
        console.log(e)
        console.log('Photo upload failed')


      } finally {
        document.getElementById('submit').disabled = false
        document.getElementById('state').innerHTML = ''

      }

    }
    image.src = re.target.result
  }
  reader.readAsDataURL(runner)

}

document.getElementById('default').addEventListener('click', handleGoDefault)
document.getElementById('submit').addEventListener('click', () => uploaded ? handleGoCustomized.call(this) : handleSubmitImages.call(this))