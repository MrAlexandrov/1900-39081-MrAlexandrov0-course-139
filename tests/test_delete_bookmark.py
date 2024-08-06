import pytest
from .conftest import DELETE_BOOKMARK_URL, ADD_BOOKMARK_URL, create_bookmark_data, create_headers_json


async def test_delete_bookmark_success(service_client, register_user, login_user, add_bookmark):
    email = "delete_bookmark_success@ya.ru"
    password = "delete_bookmark_success"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "https://delete_bookmark_success.ru"
    title = "delete_bookmark_success"
    tag = "delete_bookmark_success"
    bookmark_id = await add_bookmark(user_id, url, title, tag)

    headers = create_headers_json(user_id)
    response = await service_client.delete(DELETE_BOOKMARK_URL.format(id=bookmark_id), headers=headers)
    assert response.status == 200



async def test_delete_non_existing_bookmark(service_client, register_user, login_user):
    email = "delete_non_existing_bookmark@ya.ru"
    password = "delete_non_existing_bookmark"
    await register_user(email, password)
    user_id = await login_user(email, password)

    headers = create_headers_json(user_id)
    response = await service_client.delete(DELETE_BOOKMARK_URL.format(id=9999), headers=headers)
    assert response.status == 500
    # TODO: add 404 status handler



async def test_delete_bookmark_unauthorized(service_client):
    bookmark_id = "delete_bookmark_unauthorized"
    delete_url = DELETE_BOOKMARK_URL.format(id=bookmark_id)
    response = await service_client.delete(delete_url)
    assert response.status == 401



async def test_delete_bookmark_wrong_user(service_client, register_user, login_user):
    email1 = "first_email@ya.ru"
    password1 = "first_password"
    email2 = "second_email@ya.ru"
    password2 = "second_password"

    assert email1 != email2

    await register_user(email1, password1)
    user_id1 = await login_user(email1, password1)

    url = "https://delete.ru"
    title = "delete"
    tag = "delete"
    bookmark_data = create_bookmark_data(url, title, tag)
    headers1 = create_headers_json(user_id1)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers1)
    assert response.status == 200
    json_response = response.json()
    bookmark_id = json_response["id"]

    await register_user(email2, password2)
    user_id2 = await login_user(email2, password2)
    delete_url = DELETE_BOOKMARK_URL.format(id=bookmark_id)
    headers2 = create_headers_json(user_id2)
    response = await service_client.delete(delete_url, headers=headers2)
    assert response.status == 500
    # TODO: add 403 status handler

